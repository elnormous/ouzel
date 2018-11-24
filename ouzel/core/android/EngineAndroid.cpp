// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <system_error>
#include <cstdlib>
#include <unistd.h>
#include <android/window.h>
#include "EngineAndroid.hpp"
#include "NativeWindowAndroid.hpp"
#include "events/EventDispatcher.hpp"
#include "graphics/opengl/android/RenderDeviceOGLAndroid.hpp"

static int looperCallback(int fd, int events, void* data)
{
    if (events & ALOOPER_EVENT_INPUT)
    {
        char command;
        if (read(fd, &command, sizeof(command)) == -1)
            throw std::system_error(errno, std::system_category(), "Failed to read from pipe");

        ouzel::EngineAndroid* engineAndroid = static_cast<ouzel::EngineAndroid*>(data);

        if (command == 1)
            engineAndroid->executeAll();
    }

    return 1;
}

namespace ouzel
{
    EngineAndroid::EngineAndroid(JavaVM* initJavaVM):
        javaVM(initJavaVM), looperPipe{-1, -1}
    {
        JNIEnv* jniEnv;

        if (javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6) != JNI_OK)
            throw std::runtime_error("Failed to get JNI environment");

        uriClass = jniEnv->FindClass("android/net/Uri");
        uriClass = static_cast<jclass>(jniEnv->NewGlobalRef(uriClass));
        parseMethod = jniEnv->GetStaticMethodID(uriClass, "parse", "(Ljava/lang/String;)Landroid/net/Uri;");
        intentClass = jniEnv->FindClass("android/content/Intent");
        intentClass = static_cast<jclass>(jniEnv->NewGlobalRef(intentClass));
        intentConstructor = jniEnv->GetMethodID(intentClass, "<init>", "(Ljava/lang/String;Landroid/net/Uri;)V");
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
        if (looperPipe[0] != -1) close(looperPipe[0]);
        if (looperPipe[1] != -1) close(looperPipe[1]);
    }

    void EngineAndroid::onCreate(jobject initMainActivity)
    {
        JNIEnv* jniEnv;

        if (javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6) != JNI_OK)
            throw std::runtime_error("Failed to get JNI environment");

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

        // looper
        looper = ALooper_forThread(); // this is called on main thread, so it is safe to get the looper here
        if (!looper)
            throw std::runtime_error("Main thread has no looper");

        ALooper_acquire(looper);
        if (pipe(looperPipe) == -1)
            throw std::system_error(errno, std::system_category(), "Failed to create pipe");

        if (ALooper_addFd(looper, looperPipe[0], ALOOPER_POLL_CALLBACK, ALOOPER_EVENT_INPUT, looperCallback, this) != 1)
            throw std::runtime_error("Failed to add looper file descriptor");
    }

    void EngineAndroid::onSurfaceCreated(jobject newSurface)
    {
        JNIEnv* jniEnv;

        if (javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6) != JNI_OK)
            throw std::runtime_error("Failed to get JNI environment");

        if (surface) jniEnv->DeleteGlobalRef(surface);
        surface = jniEnv->NewGlobalRef(newSurface);

        if (active)
        {
            NativeWindowAndroid* windowAndroid = static_cast<NativeWindowAndroid*>(window->getNativeWindow());
            windowAndroid->handleSurfaceChange(surface);

            if (renderer)
            {
                graphics::RenderDevice* renderDevice = renderer->getDevice();
                if (renderDevice->getDriver() == graphics::Driver::OPENGL)
                {
                    graphics::RenderDeviceOGLAndroid* renderDeviceOGLAndroid = static_cast<graphics::RenderDeviceOGLAndroid*>(renderDevice);
                    renderDeviceOGLAndroid->reload();
                }
            }
        }
    }

    void EngineAndroid::onConfigurationChanged(jobject newConfig)
    {
        JNIEnv* jniEnv;

        if (javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6) != JNI_OK)
            throw std::runtime_error("Failed to get JNI environment");

        jint newOrientation = jniEnv->GetIntField(newConfig, orientationField);

        if (orientation != newOrientation)
        {
            orientation = newOrientation;

            std::unique_ptr<SystemEvent> event(new SystemEvent());
            event->type = Event::Type::ORIENTATION_CHANGE;

            switch (orientation)
            {
                case ORIENTATION_PORTRAIT:
                    event->orientation = SystemEvent::Orientation::PORTRAIT;
                    break;
                case ORIENTATION_LANDSCAPE:
                    event->orientation = SystemEvent::Orientation::LANDSCAPE;
                    break;
                default:
                    event->orientation = SystemEvent::Orientation::UNKNOWN;
                    break;
            }

            eventDispatcher.postEvent(std::move(event));
        }
    }

    void EngineAndroid::onSurfaceDestroyed()
    {
        JNIEnv* jniEnv;

        if (javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6) != JNI_OK)
            throw std::runtime_error("Failed to get JNI environment");

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
                if (renderDevice->getDriver() == graphics::Driver::OPENGL)
                {
                    graphics::RenderDeviceOGLAndroid* renderDeviceOGLAndroid = static_cast<graphics::RenderDeviceOGLAndroid*>(renderDevice);
                    renderDeviceOGLAndroid->destroy();
                }
            }
        }
    }

    void EngineAndroid::run()
    {
        init();
        start();
    }

    void EngineAndroid::executeOnMainThread(const std::function<void()>& func)
    {
        {
            std::unique_lock<std::mutex> lock(executeMutex);
            executeQueue.push(func);
        }

        char command = 1;
        if (write(looperPipe[1], &command, sizeof(command)) == -1)
            throw std::system_error(errno, std::system_category(), "Failed to write to pipe");
    }

    void EngineAndroid::openURL(const std::string& url)
    {
        executeOnMainThread([url, this]() {
            JNIEnv* jniEnv;

            if (javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6) != JNI_OK)
                throw std::runtime_error("Failed to get JNI environment");

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
                throw std::runtime_error("Failed to open URL");
            }
        });
    }

    void EngineAndroid::setScreenSaverEnabled(bool newScreenSaverEnabled)
    {
        Engine::setScreenSaverEnabled(newScreenSaverEnabled);

        executeOnMainThread([newScreenSaverEnabled, this]() {
            JNIEnv* jniEnv;

            if (javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6) != JNI_OK)
                throw std::runtime_error("Failed to get JNI environment");

            if (newScreenSaverEnabled)
                jniEnv->CallVoidMethod(androidWindow, clearFlagsMethod, AWINDOW_FLAG_KEEP_SCREEN_ON);
            else
                jniEnv->CallVoidMethod(androidWindow, addFlagsMethod, AWINDOW_FLAG_KEEP_SCREEN_ON);
        });
    }

    void EngineAndroid::executeAll()
    {
        std::function<void()> func;

        for (;;)
        {
            std::unique_lock<std::mutex> lock(executeMutex);
            if (executeQueue.empty()) break;
            
            func = std::move(executeQueue.front());
            executeQueue.pop();
            lock.unlock();

            if (func) func();
        }
    }

    void EngineAndroid::main()
    {
        JNIEnv* jniEnv;
        JavaVMAttachArgs attachArgs;
        attachArgs.version = JNI_VERSION_1_6;
        attachArgs.name = nullptr; // thread name
        attachArgs.group = nullptr; // thread group
        if (javaVM->AttachCurrentThread(&jniEnv, &attachArgs) != JNI_OK)
            throw std::runtime_error("Failed to attach current thread to Java VM");

        Engine::main();

        if (javaVM->DetachCurrentThread() != JNI_OK)
            throw std::runtime_error("Failed to detach current thread from Java VM");
    }
}
