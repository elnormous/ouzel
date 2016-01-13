// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <jni.h>
#include "Engine.h"
#include "Renderer.h"
#include "Utils.h"

JNIEXPORT void JNICALL Java_lv_elviss_ouzel_OuzelLibJNIWrapper_onSurfaceCreated(JNIEnv* env, jclass cls)
{
    OUZEL_UNUSED(env);
    OUZEL_UNUSED(cls);
    
    ouzel::Engine::getInstance()->begin();
}

JNIEXPORT void JNICALL Java_lv_elviss_ouzel_OuzelLibJNIWrapper_onSurfaceChanged(JNIEnv* env, jclass cls, jint width, jint height)
{
    OUZEL_UNUSED(env);
    OUZEL_UNUSED(cls);
    
    ouzel::Engine::getInstance()->getRenderer()->resize(ouzel::Size2(static_cast<float>(width), static_cast<float>(height)));
}

JNIEXPORT void JNICALL Java_lv_elviss_ouzel_OuzelLibJNIWrapper_onDrawDrame(JNIEnv* env, jclass cls)
{
    OUZEL_UNUSED(env);
    OUZEL_UNUSED(cls);
    
    ouzel::Engine::getInstance()->run();
}
