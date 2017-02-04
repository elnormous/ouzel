// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <jni.h>
#include <memory>
#include "core/android/ApplicationAndroid.h"
#include "core/android/WindowAndroid.h"
#include "core/Engine.h"
#include "input/Input.h"

std::unique_ptr<ouzel::ApplicationAndroid> application;

extern "C"
{
    jint JNI_OnLoad(JavaVM* javaVM, void*)
    {
        application.reset(new ouzel::ApplicationAndroid(javaVM));
        return JNI_VERSION_1_6;
    }

    void JNI_OnUnload(JavaVM*, void*)
    {
        application.reset();
    }

    JNIEXPORT void JNICALL Java_lv_elviss_ouzel_OuzelLibJNIWrapper_setMainActivity(JNIEnv*, jclass, jobject mainActivity)
    {
        application->setMainActivity(mainActivity);
    }

    JNIEXPORT void JNICALL Java_lv_elviss_ouzel_OuzelLibJNIWrapper_setAssetManager(JNIEnv*, jclass, jobject assetManager)
    {
        application->setAssetManager(assetManager);
    }

    JNIEXPORT void JNICALL Java_lv_elviss_ouzel_OuzelLibJNIWrapper_setSurface(JNIEnv*, jclass, jobject surface, jint width, jint height)
    {
        application->setSurface(surface);

        /*if (ouzel::sharedEngine)
        {
            ouzel::WindowAndroid* windowAndroid = static_cast<ouzel::WindowAndroid*>(ouzel::sharedEngine->getWindow());
            windowAndroid->handleResize(ouzel::Size2(static_cast<float>(width), static_cast<float>(height)));
        }*/
    }

    JNIEXPORT void JNICALL Java_lv_elviss_ouzel_OuzelLibJNIWrapper_onStart(JNIEnv*, jclass)
    {
        application->run();
    }

    JNIEXPORT void JNICALL Java_lv_elviss_ouzel_OuzelLibJNIWrapper_onPause(JNIEnv*, jclass)
    {
    }

    JNIEXPORT void JNICALL Java_lv_elviss_ouzel_OuzelLibJNIWrapper_onResume(JNIEnv*, jclass)
    {
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
