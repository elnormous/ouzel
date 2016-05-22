// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <jni.h>
#include "Engine.h"
#include "Renderer.h"
#include "FileSystem.h"
#include "Utils.h"

extern "C"
{
    JNIEXPORT void JNICALL Java_lv_elviss_ouzel_OuzelLibJNIWrapper_setAssetManager(JNIEnv* env, jclass cls, jobject assetManager)
    {
        OUZEL_UNUSED(env);
        OUZEL_UNUSED(cls);
        
        ouzel::assetManager = AAssetManager_fromJava(env, assetManager);
    }

    JNIEXPORT void JNICALL Java_lv_elviss_ouzel_OuzelLibJNIWrapper_onSurfaceCreated(JNIEnv* env, jclass cls)
    {
        OUZEL_UNUSED(env);
        OUZEL_UNUSED(cls);

        ouzelMain(ouzel::getArgs());

        if (ouzel::sharedEngine)
        {
            ouzel::sharedEngine->begin();
        }
    }

    JNIEXPORT void JNICALL Java_lv_elviss_ouzel_OuzelLibJNIWrapper_onSurfaceChanged(JNIEnv* env, jclass cls, jint width, jint height)
    {
        OUZEL_UNUSED(env);
        OUZEL_UNUSED(cls);

        if (ouzel::sharedEngine)
        {
            ouzel::sharedEngine->getWindow()->setSize(ouzel::Size2(static_cast<float>(width), static_cast<float>(height)));
        }
    }

    JNIEXPORT void JNICALL Java_lv_elviss_ouzel_OuzelLibJNIWrapper_onDrawFrame(JNIEnv* env, jclass cls)
    {
        OUZEL_UNUSED(env);
        OUZEL_UNUSED(cls);

        if (ouzel::sharedEngine)
        {
            ouzel::sharedEngine->run();
        }
    }
}
