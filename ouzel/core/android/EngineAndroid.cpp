// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <cstdlib>
#include <unistd.h>
#include <android/window.h>
#include "EngineAndroid.hpp"
#include "NativeWindowAndroid.hpp"
#include "events/EventDispatcher.hpp"
#include "graphics/opengl/android/RenderDeviceOGLAndroid.hpp"
#include "utils/Errors.hpp"

static int looperCallback(int fd, int events, void* data)
{
    if (events & ALOOPER_EVENT_INPUT)
    {
        int8_t command;
        if (read(fd, &command, sizeof(command)) == -1)
            throw ouzel::SystemError("Failed to read from pipe");

        ouzel::EngineAndroid* engineAndroid = static_cast<ouzel::EngineAndroid*>(data);

        if (command == 1)
            engineAndroid->executeAll();
    }

    return 1;
}

static int inputCallback(int fd, int events, void* data)
{
    return 1;
}

namespace ouzel
{
    EngineAndroid::EngineAndroid(JavaVM* initJavaVM):
        javaVM(initJavaVM)
    {
        std::fill(std::begin(fd), std::end(fd), 0);

        JNIEnv* jniEnv;

        if (javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6) != JNI_OK)
            throw SystemError("Failed to get JNI environment");

        uriClass = jniEnv->FindClass("android/net/Uri");
        uriClass = static_cast<jclass>(jniEnv->NewGlobalRef(uriClass));
        parseMethod = jniEnv->GetStaticMethodID(uriClass, "parse", "(Ljava/lang/String;)Landroid/net/Uri;");
        intentClass = jniEnv->FindClass("android/content/Intent");
        intentClass = static_cast<jclass>(jniEnv->NewGlobalRef(intentClass));
        intentConstructor = jniEnv->GetMethodID(intentClass, "<init>", "(Ljava/lang/String;Landroid/net/Uri;)V");

        // looper
        looper = ALooper_forThread(); // this is called on main thread, so it is safe to get the looper here
        if (!looper)
            throw SystemError("Main thread has no looper");

        ALooper_acquire(looper);
        if (pipe(fd) != 0)
            throw SystemError("Failed to create pipe, error: " + std::to_string(errno));

        if (ALooper_addFd(looper, fd[0], ALOOPER_POLL_CALLBACK, ALOOPER_EVENT_INPUT, looperCallback, this) != 1)
            throw SystemError("Failed to add looper file descriptor");
    }

    EngineAndroid::~EngineAndroid()
    {
        if (updateThread.joinable()) updateThread.join();

        JNIEnv* jniEnv;

        if (javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6) == JNI_OK)
        {
            if (mainActivity) jniEnv->DeleteGlobalRef(mainActivity);
            if (androidWindow) jniEnv->DeleteGlobalRef(androidWindow);
            if (surface) jniEnv->DeleteGlobalRef(surface);
            if (intentClass) jniEnv->DeleteGlobalRef(intentClass);
            if (uriClass) jniEnv->DeleteGlobalRef(uriClass);
        }

        if (looper) ALooper_release(looper);
        if (fd[0]) close(fd[0]);
        if (fd[1]) close(fd[1]);
    }

    /*void EngineAndroid::onCreate(jobject initMainActivity)
    {
        JNIEnv* jniEnv;

        if (javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6) != JNI_OK)
            throw SystemError("Failed to get JNI environment");

        mainActivity = jniEnv->NewGlobalRef(initMainActivity);

        jclass mainActivityClass = jniEnv->GetObjectClass(mainActivity);
        startActivityMethod = jniEnv->GetMethodID(mainActivityClass, "startActivity", "(Landroid/content/Intent;)V");

        // get resources
        jclass resourcesClass = jniEnv->FindClass("android/content/res/Resources");
        jmethodID getResourcesMethod = jniEnv->GetMethodID(mainActivityClass, "getResources", "()Landroid/content/res/Resources;");
        jobject resourcesObject = jniEnv->CallObjectMethod(mainActivity, getResourcesMethod);
        jmethodID getConfigurationMethod = jniEnv->GetMethodID(resourcesClass, "getConfiguration", "()Landroid/content/res/Configuration;");

        // get configuration
        configurationClass = jniEnv->FindClass("android/content/res/Configuration");
        jobject configurationObject = jniEnv->CallObjectMethod(resourcesObject, getConfigurationMethod);

        orientationField = jniEnv->GetFieldID(configurationClass, "orientation", "I");
        orientation = jniEnv->GetIntField(configurationObject, orientationField);

        // get asset manager
        jmethodID getAssetsMethod = jniEnv->GetMethodID(mainActivityClass, "getAssets", "()Landroid/content/res/AssetManager;");
        jobject assetManagerObject = jniEnv->CallObjectMethod(mainActivity, getAssetsMethod);
        assetManager = AAssetManager_fromJava(jniEnv, assetManagerObject);

        // get window
        jmethodID getWindowMethod = jniEnv->GetMethodID(mainActivityClass, "getWindow", "()Landroid/view/Window;");
        androidWindow = jniEnv->CallObjectMethod(mainActivity, getWindowMethod);
        androidWindow = jniEnv->NewGlobalRef(androidWindow);

        jclass windowClass = jniEnv->FindClass("android/view/Window");
        addFlagsMethod = jniEnv->GetMethodID(windowClass, "addFlags", "(I)V");
        clearFlagsMethod = jniEnv->GetMethodID(windowClass, "clearFlags", "(I)V");

        // File class
        jclass fileClass = jniEnv->FindClass("java/io/File");
        jmethodID getAbsolutePathMethod = jniEnv->GetMethodID(fileClass, "getAbsolutePath", "()Ljava/lang/String;");

        // dataDir
        jmethodID getFilesDirMethod = jniEnv->GetMethodID(mainActivityClass, "getFilesDir", "()Ljava/io/File;");
        jobject filesDirFile = jniEnv->CallObjectMethod(mainActivity, getFilesDirMethod);

        jstring filesDirString = static_cast<jstring>(jniEnv->CallObjectMethod(filesDirFile, getAbsolutePathMethod));
        const char* filesDirCString = jniEnv->GetStringUTFChars(filesDirString, 0);
        filesDirectory = filesDirCString;
        jniEnv->ReleaseStringUTFChars(filesDirString, filesDirCString);

        // cacheDir
        jmethodID getCacheDirMethod = jniEnv->GetMethodID(mainActivityClass, "getCacheDir", "()Ljava/io/File;");
        jobject cacheDirFile = jniEnv->CallObjectMethod(mainActivity, getCacheDirMethod);

        jstring cacheDirString = static_cast<jstring>(jniEnv->CallObjectMethod(cacheDirFile, getAbsolutePathMethod));
        const char* cacheDirCString = jniEnv->GetStringUTFChars(cacheDirString, 0);
        cacheDirectory = cacheDirCString;
        jniEnv->ReleaseStringUTFChars(cacheDirString, cacheDirCString);
    }*/

    void EngineAndroid::handleWindowCreate(ANativeWindow* nativeWindow)
    {
        /*JNIEnv* jniEnv;

        if (javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6) != JNI_OK)
            throw SystemError("Failed to get JNI environment");

        if (surface) jniEnv->DeleteGlobalRef(surface);
        surface = jniEnv->NewGlobalRef(newSurface);

        if (active)
        {
            NativeWindowAndroid* windowAndroid = static_cast<NativeWindowAndroid*>(window->getNativeWindow());
            windowAndroid->handleSurfaceChange(surface);

            if (renderer)
            {
                graphics::RenderDevice* renderDevice = renderer->getDevice();
                if (renderDevice->getDriver() == graphics::Renderer::Driver::OPENGL)
                {
                    graphics::RenderDeviceOGLAndroid* renderDeviceOGLAndroid = static_cast<graphics::RenderDeviceOGLAndroid*>(renderDevice);
                    //renderDeviceOGLAndroid->reload();
                }
            }
        }*/
    }

    void EngineAndroid::handleWindowDestroy()
    {
        /*JNIEnv* jniEnv;

        if (javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6) != JNI_OK)
            throw SystemError("Failed to get JNI environment");

        if (surface)
        {
            jniEnv->DeleteGlobalRef(surface);
            surface = nullptr;
        }

        if (active)
        {
            NativeWindowAndroid* windowAndroid = static_cast<NativeWindowAndroid*>(window->getNativeWindow());
            windowAndroid->handleSurfaceDestroy();

            if (renderer)
            {
                graphics::RenderDevice* renderDevice = renderer->getDevice();
                if (renderDevice->getDriver() == graphics::Renderer::Driver::OPENGL)
                {
                    graphics::RenderDeviceOGLAndroid* renderDeviceOGLAndroid = static_cast<graphics::RenderDeviceOGLAndroid*>(renderDevice);
                    renderDeviceOGLAndroid->destroy();
                }
            }
        }*/
    }

    void EngineAndroid::handleConfigurationChange()
    {
        /*AConfiguration_fromAssetManager();

        ouzel::NativeWindowAndroid* windowAndroid = static_cast<ouzel::NativeWindowAndroid*>(engine->getWindow()->getNativeWindow());
        windowAndroid->handleResize(ouzel::Size2(static_cast<float>(width), static_cast<float>(height)));

        JNIEnv* jniEnv;

        if (javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6) != JNI_OK)
            throw SystemError("Failed to get JNI environment");

        jint newOrientation = jniEnv->GetIntField(newConfig, orientationField);

        if (orientation != newOrientation)
        {
            orientation = newOrientation;

            ouzel::Event event;
            event.type = ouzel::Event::Type::ORIENTATION_CHANGE;

            switch (orientation)
            {
                case ORIENTATION_PORTRAIT:
                    event.systemEvent.orientation = ouzel::SystemEvent::Orientation::PORTRAIT;
                    break;
                case ORIENTATION_LANDSCAPE:
                    event.systemEvent.orientation = ouzel::SystemEvent::Orientation::LANDSCAPE;
                    break;
                default:
                    event.systemEvent.orientation = ouzel::SystemEvent::Orientation::UNKNOWN;
                    break;
            }

            eventDispatcher.postEvent(event);
        }*/
    }

    void EngineAndroid::handleLowMemory()
    {
        ouzel::Event event;
        event.type = ouzel::Event::Type::LOW_MEMORY;

        eventDispatcher.postEvent(event);
    }

    void EngineAndroid::handleInputQueueCreate(AInputQueue* queue)
    {
        //AInputQueue_attachLooper(queue, looper, LOOPER_ID_INPUT, inputCallback, this);
    }

    void EngineAndroid::handleInputQueueDestroy(AInputQueue* queue)
    {
        AInputQueue_detachLooper(queue);
    }

    void EngineAndroid::run()
    {
        init();
        start();
    }

    void EngineAndroid::executeOnMainThread(const std::function<void(void)>& func)
    {
        {
            std::unique_lock<std::mutex> lock(executeMutex);
            executeQueue.push(func);
        }

        int8_t command = 1;
        if (write(fd[1], &command, sizeof(command)) == -1)
            throw SystemError("Failed to write to pipe");
    }

    void EngineAndroid::openURL(const std::string& url)
    {
        executeOnMainThread([url, this]() {
            JNIEnv* jniEnv;

            if (javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6) != JNI_OK)
                throw SystemError("Failed to get JNI environment");

            jstring actionString = jniEnv->NewStringUTF("android.intent.action.VIEW");
            jstring urlString = jniEnv->NewStringUTF(url.c_str());
            jobject uri = jniEnv->CallStaticObjectMethod(uriClass, parseMethod, urlString);
            jobject intentObject = jniEnv->NewObject(intentClass, intentConstructor, actionString, uri);

            jniEnv->CallVoidMethod(mainActivity, startActivityMethod, intentObject);

            jniEnv->DeleteLocalRef(intentObject);
            jniEnv->DeleteLocalRef(uri);
            jniEnv->DeleteLocalRef(urlString);
            jniEnv->DeleteLocalRef(actionString);

            if (jniEnv->ExceptionCheck())
            {
                jniEnv->ExceptionClear();
                throw SystemError("Failed to open URL");
            }
        });
    }

    void EngineAndroid::setScreenSaverEnabled(bool newScreenSaverEnabled)
    {
        Engine::setScreenSaverEnabled(newScreenSaverEnabled);

        executeOnMainThread([newScreenSaverEnabled, this]() {
            JNIEnv* jniEnv;

            if (javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6) != JNI_OK)
                throw SystemError("Failed to get JNI environment");

            if (newScreenSaverEnabled)
                jniEnv->CallVoidMethod(androidWindow, clearFlagsMethod, AWINDOW_FLAG_KEEP_SCREEN_ON);
            else
                jniEnv->CallVoidMethod(androidWindow, addFlagsMethod, AWINDOW_FLAG_KEEP_SCREEN_ON);
        });
    }

    void EngineAndroid::executeAll()
    {
        std::function<void(void)> func;

        {
            std::unique_lock<std::mutex> lock(executeMutex);

            if (!executeQueue.empty())
            {
                func = std::move(executeQueue.front());
                executeQueue.pop();
            }
        }

        if (func) func();
    }

    void EngineAndroid::main()
    {
        JNIEnv* jniEnv;
        JavaVMAttachArgs attachArgs;
        attachArgs.version = JNI_VERSION_1_6;
        attachArgs.name = nullptr; // thread name
        attachArgs.group = nullptr; // thread group
        if (javaVM->AttachCurrentThread(&jniEnv, &attachArgs) != JNI_OK)
            throw SystemError("Failed to attach current thread to Java VM");

        Engine::main();

        if (javaVM->DetachCurrentThread() != JNI_OK)
            throw SystemError("Failed to detach current thread from Java VM");
    }
}
