// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstdlib>
#include <android/window.h>
#include "ApplicationAndroid.h"
#include "core/Engine.h"
#include "utils/Log.h"

namespace ouzel
{
    ApplicationAndroid::ApplicationAndroid(JavaVM* aJavaVM):
        javaVM(aJavaVM)
    {
        JNIEnv* jniEnv;

        if (javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6) != JNI_OK)
        {
            Log(Log::Level::ERR) << "Failed to get JNI environment";
            return;
        }

        uriClass = reinterpret_cast<jclass>(jniEnv->NewGlobalRef(jniEnv->FindClass("android/net/Uri")));
        parseMethod = jniEnv->GetStaticMethodID(uriClass, "parse", "(Ljava/lang/String;)Landroid/net/Uri;");
        intentClass = reinterpret_cast<jclass>(jniEnv->NewGlobalRef(jniEnv->FindClass("android/content/Intent")));
        intentConstructor = jniEnv->GetMethodID(intentClass, "<init>", "(Ljava/lang/String;Landroid/net/Uri;)V");
    }

    ApplicationAndroid::~ApplicationAndroid()
    {
        if (updateThread.joinable()) updateThread.join();

        JNIEnv* jniEnv;

        if (javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6) != JNI_OK)
        {
            Log(Log::Level::ERR) << "Failed to get JNI environment";
            return;
        }

        if (mainActivity) jniEnv->DeleteGlobalRef(mainActivity);
        if (window) jniEnv->DeleteGlobalRef(window);
        if (surface) jniEnv->DeleteGlobalRef(surface);
        if (intentClass) jniEnv->DeleteGlobalRef(intentClass);
        if (uriClass) jniEnv->DeleteGlobalRef(uriClass);
    }

    void ApplicationAndroid::onCreate(jobject aMainActivity)
    {
        JNIEnv* jniEnv;

        if (javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6) != JNI_OK)
        {
            Log(Log::Level::ERR) << "Failed to get JNI environment";
            return;
        }

        mainActivity = jniEnv->NewGlobalRef(aMainActivity);

        jclass mainActivityClass = jniEnv->GetObjectClass(mainActivity);
        startActivityMethod = jniEnv->GetMethodID(mainActivityClass, "startActivity", "(Landroid/content/Intent;)V");

        // get asset manager
        jmethodID getAssetsMethod = jniEnv->GetMethodID(mainActivityClass, "getAssets", "()Landroid/content/res/AssetManager;");
        jobject assetManagerObject = jniEnv->CallObjectMethod(mainActivity, getAssetsMethod);
        assetManager = AAssetManager_fromJava(jniEnv, assetManagerObject);

        // get window
        jmethodID getWindowMethod = jniEnv->GetMethodID(mainActivityClass, "getWindow", "()Landroid/view/Window;");
        window = jniEnv->CallObjectMethod(mainActivity, getWindowMethod);

        jclass windowClass = jniEnv->GetObjectClass(window);
        addFlagsMethod = jniEnv->GetMethodID(windowClass, "addFlags", "(I)V");
        clearFlagsMethod = jniEnv->GetMethodID(windowClass, "clearFlags", "(I)V");

        // get package manager
        jmethodID getPackageManagerMethod = jniEnv->GetMethodID(mainActivityClass, "getPackageManager", "()Landroid/content/pm/PackageManager;");
        jobject packageManager  = jniEnv->CallObjectMethod(mainActivity, getPackageManagerMethod);
        jclass packageManagerClass = jniEnv->GetObjectClass(packageManager);
        jmethodID getPackageInfoMethod = jniEnv->GetMethodID(packageManagerClass, "getPackageInfo", "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");

        // get package name
        jmethodID getPackageNameMethod = jniEnv->GetMethodID(mainActivityClass,"getPackageName","()Ljava/lang/String;");
        jstring packageName = static_cast<jstring>(jniEnv->CallObjectMethod(mainActivity, getPackageNameMethod));

        // get package info
        jobject packageInfo = jniEnv->CallObjectMethod(packageManager, getPackageInfoMethod, packageName, 0);
        jclass packageInfoClass = jniEnv->GetObjectClass(packageInfo);

        // get application info
        jfieldID applicationInfoField = jniEnv->GetFieldID(packageInfoClass, "applicationInfo", "Landroid/content/pm/ApplicationInfo;");
        jobject applicationInfo = jniEnv->GetObjectField(packageInfo, applicationInfoField);
        jclass applicationInfoClass = jniEnv->GetObjectClass(applicationInfo);

        // dataDir
        jfieldID dataDirField = jniEnv->GetFieldID(applicationInfoClass, "dataDir", "Ljava/lang/String;");
        jstring dataDirString = static_cast<jstring>(jniEnv->GetObjectField(applicationInfo, dataDirField));

        const char* dataDirCString = jniEnv->GetStringUTFChars(dataDirString, 0);

        dataDirectory = dataDirCString;

        jniEnv->ReleaseStringUTFChars(dataDirString, dataDirCString);

        Log() << "DIIIIIIIIRECTORYYYYYYYYYY " << dataDirectory;
    }

    void ApplicationAndroid::setSurface(jobject aSurface)
    {
        JNIEnv* jniEnv;

        if (javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6) != JNI_OK)
        {
            Log(Log::Level::ERR) << "Failed to get JNI environment";
            return;
        }

        surface = jniEnv->NewGlobalRef(aSurface);
    }

    int ApplicationAndroid::run()
    {
        updateThread = std::thread(&ApplicationAndroid::update, this);

        return EXIT_SUCCESS;
    }

    void ApplicationAndroid::execute(const std::function<void(void)>& func)
    {
        if (func)
        {
            // TODO: implement running on UI thread (Activity.runOnUiThread)
        }
    }

    bool ApplicationAndroid::openURL(const std::string& url)
    {
        JNIEnv* jniEnv;

        if (javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6) != JNI_OK)
        {
            Log(Log::Level::ERR) << "Failed to get JNI environment";
            return false;
        }

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
            Log(Log::Level::ERR) << "Failed to open URL";
            jniEnv->ExceptionClear();
            return false;
        }

        return true;
    }

    void ApplicationAndroid::setScreenSaverEnabled(bool newScreenSaverEnabled)
    {
        Application::setScreenSaverEnabled(newScreenSaverEnabled);

        execute([newScreenSaverEnabled, this]() {
            JNIEnv* jniEnv;

            if (javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6) != JNI_OK)
            {
                Log(Log::Level::ERR) << "Failed to get JNI environment";
                return;
            }

            if (newScreenSaverEnabled)
            {
                jniEnv->CallVoidMethod(window, clearFlagsMethod, AWINDOW_FLAG_KEEP_SCREEN_ON);
            }
            else
            {
                jniEnv->CallVoidMethod(window, addFlagsMethod, AWINDOW_FLAG_KEEP_SCREEN_ON);
            }
        });
    }

    void ApplicationAndroid::update()
    {
        JNIEnv* jniEnv;
        JavaVMAttachArgs attachArgs;
        attachArgs.version = JNI_VERSION_1_6;
        attachArgs.name = NULL; // thread name
        attachArgs.group = NULL; // thread group
        if (javaVM->AttachCurrentThread(&jniEnv, &attachArgs) != JNI_OK)
        {
            Log(Log::Level::ERR) << "Failed to attach current thread to Java VM";
        }

        init();

        if (sharedEngine)
        {
            sharedEngine->start();
        }
        else
        {
            ::exit(EXIT_FAILURE);
        }

        while (active)
        {
            if (!sharedEngine->draw())
            {
                break;
            }
        }

        if (sharedEngine)
        {
            sharedEngine->stop();
        }

        if (javaVM->DetachCurrentThread() != JNI_OK)
        {
            Log(Log::Level::ERR) << "Failed to detach current thread from Java VM";
        }

        ::exit(EXIT_SUCCESS);
    }
}
