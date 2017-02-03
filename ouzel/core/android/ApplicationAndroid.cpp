// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstdlib>
#include "ApplicationAndroid.h"
#include "core/Engine.h"
#include "utils/Log.h"

namespace ouzel
{
    ApplicationAndroid::ApplicationAndroid(JavaVM* aJavaVM):
        javaVM(aJavaVM)
    {
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

        jniEnv->DeleteGlobalRef(mainActivity);
    }

    void ApplicationAndroid::setMainActivity(jobject aMainActivity)
    {
        JNIEnv* jniEnv;

        if (javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6) != JNI_OK)
        {
            Log(Log::Level::ERR) << "Failed to get JNI environment";
            return;
        }

        mainActivity = jniEnv->NewGlobalRef(aMainActivity);
        jclass mainActivityClass = jniEnv->GetObjectClass(mainActivity);

        openURLMethod = jniEnv->GetMethodID(mainActivityClass, "openURL", "(Ljava/lang/String;)V");
    }

    void ApplicationAndroid::setAssetManager(jobject aAssetManager)
    {
        JNIEnv* jniEnv;

        if (javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6) != JNI_OK)
        {
            Log(Log::Level::ERR) << "Failed to get JNI environment";
            return;
        }

        assetManager = AAssetManager_fromJava(jniEnv, aAssetManager);
    }

    void ApplicationAndroid::setSurface(jobject aSurface)
    {
        surface = aSurface;
    }

    int ApplicationAndroid::run()
    {
        ouzelMain(ouzel::sharedApplication->getArgs());

        if (!sharedEngine)
        {
            return EXIT_FAILURE;
        }

        updateThread = std::thread(&ApplicationAndroid::update, this);

        return EXIT_SUCCESS;
    }

    bool ApplicationAndroid::openURL(const std::string& url)
    {
        JNIEnv* jniEnv;

        if (javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6) != JNI_OK)
        {
            Log(Log::Level::ERR) << "Failed to get JNI environment";
            return false;
        }

        jstring urlString = jniEnv->NewStringUTF(url.c_str());
        jniEnv->CallVoidMethod(mainActivity, openURLMethod, urlString);
        jniEnv->DeleteLocalRef(urlString);

        return true;
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

        while (active)
        {
            executeAll();

            if (!sharedEngine->draw())
            {
                break;
            }
        }

        if (ouzel::sharedEngine)
        {
            ouzel::sharedEngine->exitUpdateThread();
        }

        if (javaVM->DetachCurrentThread() != JNI_OK)
        {
            Log(Log::Level::ERR) << "Failed to detach current thread from Java VM";
        }

        ::exit(EXIT_SUCCESS);
    }
}
