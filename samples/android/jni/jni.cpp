// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <jni.h>
#include "Engine.h"
#include "Renderer.h"
#include "FileSystem.h"
#include "Input.h"
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
    
    JNIEXPORT void JNICALL Java_lv_elviss_ouzel_OuzelLibJNIWrapper_handleActionDown(JNIEnv* env, jclass cls, jint pointerId, jfloat x, jfloat y)
    {
        OUZEL_UNUSED(env);
        OUZEL_UNUSED(cls);

        ouzel::sharedEngine->getInput()->touchBegin(static_cast<uint64_t>(pointerId),
                                                    ouzel::sharedEngine->getRenderer()->viewToScreenLocation(ouzel::Vector2(x, y)));
    }
    
    JNIEXPORT void JNICALL Java_lv_elviss_ouzel_OuzelLibJNIWrapper_handleActionMove(JNIEnv* env, jclass cls, jint pointerId, jfloat x, jfloat y)
    {
        OUZEL_UNUSED(env);
        OUZEL_UNUSED(cls);

        ouzel::sharedEngine->getInput()->touchMove(static_cast<uint64_t>(pointerId),
                                                   ouzel::sharedEngine->getRenderer()->viewToScreenLocation(ouzel::Vector2(x, y)));
    }
    
    JNIEXPORT void JNICALL Java_lv_elviss_ouzel_OuzelLibJNIWrapper_handleActionUp(JNIEnv* env, jclass cls, jint pointerId, jfloat x, jfloat y)
    {
        OUZEL_UNUSED(env);
        OUZEL_UNUSED(cls);

        ouzel::sharedEngine->getInput()->touchEnd(static_cast<uint64_t>(pointerId),
                                                  ouzel::sharedEngine->getRenderer()->viewToScreenLocation(ouzel::Vector2(x, y)));
    }
    
    JNIEXPORT void JNICALL Java_lv_elviss_ouzel_OuzelLibJNIWrapper_handleActionCancel(JNIEnv* env, jclass cls, jint pointerId, jfloat x, jfloat y)
    {
        OUZEL_UNUSED(env);
        OUZEL_UNUSED(cls);

        ouzel::sharedEngine->getInput()->touchCancel(static_cast<uint64_t>(pointerId),
                                                     ouzel::sharedEngine->getRenderer()->viewToScreenLocation(ouzel::Vector2(x, y)));
    }
}
