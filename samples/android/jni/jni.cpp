// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <jni.h>
#include "core/android/ApplicationAndroid.h"
#include "core/android/WindowAndroid.h"
#include "core/Engine.h"
#include "core/Window.h"
#include "files/FileSystem.h"
#include "input/Input.h"
#include "utils/Utils.h"

ouzel::ApplicationAndroid application;

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

        application.run();
    }

    JNIEXPORT void JNICALL Java_lv_elviss_ouzel_OuzelLibJNIWrapper_onSurfaceChanged(JNIEnv* env, jclass cls, jint width, jint height)
    {
        OUZEL_UNUSED(env);
        OUZEL_UNUSED(cls);

        if (ouzel::sharedEngine)
        {
            ouzel::WindowAndroid* windowAndroid = static_cast<ouzel::WindowAndroid*>(ouzel::sharedEngine->getWindow());
            windowAndroid->handleResize(ouzel::Size2(static_cast<float>(width), static_cast<float>(height)));
        }
    }

    JNIEXPORT void JNICALL Java_lv_elviss_ouzel_OuzelLibJNIWrapper_onDrawFrame(JNIEnv* env, jclass cls)
    {
        OUZEL_UNUSED(env);
        OUZEL_UNUSED(cls);

        if (ouzel::sharedEngine)
        {
            ouzel::sharedEngine->draw();
        }
    }

    JNIEXPORT void JNICALL Java_lv_elviss_ouzel_OuzelLibJNIWrapper_handleActionDown(JNIEnv* env, jclass cls, jint pointerId, jfloat x, jfloat y)
    {
        OUZEL_UNUSED(env);
        OUZEL_UNUSED(cls);

        ouzel::sharedEngine->getInput()->touchBegin(static_cast<uint64_t>(pointerId),
                                                    ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)));
    }

    JNIEXPORT void JNICALL Java_lv_elviss_ouzel_OuzelLibJNIWrapper_handleActionMove(JNIEnv* env, jclass cls, jint pointerId, jfloat x, jfloat y)
    {
        OUZEL_UNUSED(env);
        OUZEL_UNUSED(cls);

        ouzel::sharedEngine->getInput()->touchMove(static_cast<uint64_t>(pointerId),
                                                   ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)));
    }

    JNIEXPORT void JNICALL Java_lv_elviss_ouzel_OuzelLibJNIWrapper_handleActionUp(JNIEnv* env, jclass cls, jint pointerId, jfloat x, jfloat y)
    {
        OUZEL_UNUSED(env);
        OUZEL_UNUSED(cls);

        ouzel::sharedEngine->getInput()->touchEnd(static_cast<uint64_t>(pointerId),
                                                  ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)));
    }

    JNIEXPORT void JNICALL Java_lv_elviss_ouzel_OuzelLibJNIWrapper_handleActionCancel(JNIEnv* env, jclass cls, jint pointerId, jfloat x, jfloat y)
    {
        OUZEL_UNUSED(env);
        OUZEL_UNUSED(cls);

        ouzel::sharedEngine->getInput()->touchCancel(static_cast<uint64_t>(pointerId),
                                                     ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)));
    }
}
