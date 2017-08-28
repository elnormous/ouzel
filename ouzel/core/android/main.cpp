// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <jni.h>
#include <memory>
#include "core/android/EngineAndroid.hpp"
#include "core/android/WindowAndroid.hpp"
#include "core/Engine.hpp"
#include "events/EventDispatcher.hpp"
#include "input/android/InputAndroid.hpp"

static std::unique_ptr<ouzel::EngineAndroid> engine;

extern "C" JNIEXPORT jint JNIEXPORT JNI_OnLoad(JavaVM* javaVM, void*)
{
    engine.reset(new ouzel::EngineAndroid(javaVM));
    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNIEXPORT JNI_OnUnload(JavaVM*, void*)
{
    engine.reset();
}

extern "C" JNIEXPORT void JNICALL Java_org_ouzelengine_OuzelLibJNIWrapper_onCreate(JNIEnv*, jclass, jobject mainActivity)
{
    engine->onCreate(mainActivity);
}

extern "C" JNIEXPORT void JNICALL Java_org_ouzelengine_OuzelLibJNIWrapper_onSurfaceCreated(JNIEnv*, jclass, jobject surface)
{
    engine->setSurface(surface);

    if (!engine->isActive()) engine->run();
}

extern "C" JNIEXPORT void JNICALL Java_org_ouzelengine_OuzelLibJNIWrapper_onSurfaceDestroyed(JNIEnv*, jclass)
{
    engine->setSurface(nullptr);
}

extern "C" JNIEXPORT void JNICALL Java_org_ouzelengine_OuzelLibJNIWrapper_onSurfaceChanged(JNIEnv*, jclass, jobject surface, jint width, jint height)
{
    engine->setSurface(surface);
    ouzel::WindowAndroid* windowAndroid = static_cast<ouzel::WindowAndroid*>(engine->getWindow());
    windowAndroid->handleResize(ouzel::Size2(static_cast<float>(width), static_cast<float>(height)));
}

extern "C" JNIEXPORT void JNICALL Java_org_ouzelengine_OuzelLibJNIWrapper_onStart(JNIEnv*, jclass)
{
    // Do nothing
}

extern "C" JNIEXPORT void JNICALL Java_org_ouzelengine_OuzelLibJNIWrapper_onPause(JNIEnv*, jclass)
{
    engine->pause();
}

extern "C" JNIEXPORT void JNICALL Java_org_ouzelengine_OuzelLibJNIWrapper_onResume(JNIEnv*, jclass)
{
    engine->resume();
}

extern "C" JNIEXPORT void JNICALL Java_org_ouzelengine_OuzelLibJNIWrapper_onLowMemory(JNIEnv*, jclass)
{
    ouzel::Event event;
    event.type = ouzel::Event::Type::LOW_MEMORY;

    engine->getEventDispatcher()->postEvent(event);
}

extern "C" JNIEXPORT jboolean JNICALL Java_org_ouzelengine_OuzelLibJNIWrapper_onTouchEvent(JNIEnv*, jclass, jobject event)
{
    ouzel::input::InputAndroid* inputAndroid = static_cast<ouzel::input::InputAndroid*>(engine->getInput());
    return inputAndroid->handleTouchEvent(event);
}

extern "C" JNIEXPORT void JNICALL Java_org_ouzelengine_OuzelLibJNIWrapper_onKeyDown(JNIEnv*, jclass, jint keyCode)
{
    engine->getInput()->keyPress(ouzel::input::InputAndroid::convertKeyCode(keyCode), 0);
}

extern "C" JNIEXPORT void JNICALL Java_org_ouzelengine_OuzelLibJNIWrapper_onKeyUp(JNIEnv*, jclass, jint keyCode)
{
    engine->getInput()->keyRelease(ouzel::input::InputAndroid::convertKeyCode(keyCode), 0);
}
