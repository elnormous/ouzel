// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <jni.h>
#include "core/android/ApplicationAndroid.h"
#include "core/android/WindowAndroid.h"
#include "core/Engine.h"
#include "input/Input.h"

ouzel::ApplicationAndroid application;

extern "C"
{
    JNIEXPORT void JNICALL Java_lv_elviss_ouzel_OuzelLibJNIWrapper_onCreated(JNIEnv* env, jclass, jobject assetManager)
    {
        ouzel::assetManager = AAssetManager_fromJava(env, assetManager);
    }

    JNIEXPORT void JNICALL Java_lv_elviss_ouzel_OuzelLibJNIWrapper_onSurfaceCreated(JNIEnv*, jclass)
    {
        application.run();
    }
    JNIEXPORT void JNICALL Java_lv_elviss_ouzel_OuzelLibJNIWrapper_onSurfaceChanged(JNIEnv*, jclass, jint width, jint height)
    {
        if (ouzel::sharedEngine)
        {
            ouzel::WindowAndroid* windowAndroid = static_cast<ouzel::WindowAndroid*>(ouzel::sharedEngine->getWindow());
            windowAndroid->handleResize(ouzel::Size2(static_cast<float>(width), static_cast<float>(height)));
        }
    }

    JNIEXPORT void JNICALL Java_lv_elviss_ouzel_OuzelLibJNIWrapper_onDrawFrame(JNIEnv*, jclass)
    {
        if (ouzel::sharedEngine)
        {
            ouzel::sharedEngine->draw();
        }
    }

    JNIEXPORT void JNICALL Java_lv_elviss_ouzel_OuzelLibJNIWrapper_handleActionDown(JNIEnv*, jclass, jint pointerId, jfloat x, jfloat y)
    {
        ouzel::sharedEngine->getInput()->touchBegin(static_cast<uint64_t>(pointerId),
                                                    ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)));
    }

    JNIEXPORT void JNICALL Java_lv_elviss_ouzel_OuzelLibJNIWrapper_handleActionMove(JNIEnv*, jclass, jint pointerId, jfloat x, jfloat y)
    {
        ouzel::sharedEngine->getInput()->touchMove(static_cast<uint64_t>(pointerId),
                                                   ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)));
    }

    JNIEXPORT void JNICALL Java_lv_elviss_ouzel_OuzelLibJNIWrapper_handleActionUp(JNIEnv*, jclass, jint pointerId, jfloat x, jfloat y)
    {
        ouzel::sharedEngine->getInput()->touchEnd(static_cast<uint64_t>(pointerId),
                                                  ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)));
    }

    JNIEXPORT void JNICALL Java_lv_elviss_ouzel_OuzelLibJNIWrapper_handleActionCancel(JNIEnv*, jclass, jint pointerId, jfloat x, jfloat y)
    {
        ouzel::sharedEngine->getInput()->touchCancel(static_cast<uint64_t>(pointerId),
                                                     ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)));
    }
}
