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
        JNIEnv* jniEnv;

        if (javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6) != JNI_OK)
        {
            Log(Log::Level::ERR) << "Failed to get JNI environment";
            return;
        }

        jniEnv->DeleteGlobalRef(mainActivity);
    }

    void ApplicationAndroid::setActivity(jobject aMainActivity, jobject aAssetManager)
    {
        JNIEnv* jniEnv;

        if (javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6) != JNI_OK)
        {
            Log(Log::Level::ERR) << "Failed to get JNI environment";
            return;
        }

        mainActivity = jniEnv->NewGlobalRef(aMainActivity);
        jclass mainActivityClass = jniEnv->GetObjectClass(mainActivity);

        createSurfaceMethod = jniEnv->GetMethodID(mainActivityClass, "createSurface", "(IIIIIIII)V");
        openURLMethod = jniEnv->GetMethodID(mainActivityClass, "openURL", "(Ljava/lang/String;)V");

        assetManager = AAssetManager_fromJava(jniEnv, aAssetManager);
    }

    int ApplicationAndroid::run()
    {
        ouzelMain(ouzel::sharedApplication->getArgs());

        if (!sharedEngine)
        {
            return EXIT_FAILURE;
        }

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

    bool ApplicationAndroid::step()
    {
        executeAll();

        if (!sharedEngine->draw())
        {
            return false;
        }

        return active;
    }
}
