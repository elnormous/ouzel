// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <memory>
#include <jni.h>
#include "core/android/EngineAndroid.hpp"
#include "core/android/NativeWindowAndroid.hpp"
#include "core/Engine.hpp"
#include "events/EventDispatcher.hpp"
#include "input/android/InputSystemAndroid.hpp"
#include "utils/Log.hpp"

static std::unique_ptr<ouzel::EngineAndroid> engine;

extern "C" JNIEXPORT jint JNIEXPORT JNI_OnLoad(JavaVM* javaVM, void*)
{
    engine.reset(new ouzel::EngineAndroid(javaVM));
    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNIEXPORT JNI_OnUnload(JavaVM*, void*)
{
    engine->exit();
    engine.reset();
}

extern "C" JNIEXPORT void JNICALL Java_org_ouzelengine_OuzelLibJNIWrapper_onCreate(JNIEnv*, jclass, jobject mainActivity)
{
    engine->onCreate(mainActivity);
}

extern "C" JNIEXPORT void JNICALL Java_org_ouzelengine_OuzelLibJNIWrapper_onSurfaceCreated(JNIEnv*, jclass, jobject surface)
{
    try
    {
        engine->onSurfaceCreated(surface);

        if (!engine->isActive()) engine->run();
    }
    catch (const std::exception& e)
    {
        ouzel::Log(ouzel::Log::Level::ERR) << e.what();
    }
}

extern "C" JNIEXPORT void JNICALL Java_org_ouzelengine_OuzelLibJNIWrapper_onSurfaceDestroyed(JNIEnv*, jclass)
{
    engine->onSurfaceDestroyed();
}

extern "C" JNIEXPORT void JNICALL Java_org_ouzelengine_OuzelLibJNIWrapper_onSurfaceChanged(JNIEnv*, jclass, jobject, jint width, jint height)
{
    ouzel::NativeWindowAndroid* windowAndroid = static_cast<ouzel::NativeWindowAndroid*>(engine->getWindow()->getNativeWindow());
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

extern "C" JNIEXPORT void JNICALL Java_org_ouzelengine_OuzelLibJNIWrapper_onConfigurationChanged(JNIEnv*, jclass, jobject newConfig)
{
    engine->onConfigurationChanged(newConfig);
}

extern "C" JNIEXPORT void JNICALL Java_org_ouzelengine_OuzelLibJNIWrapper_onLowMemory(JNIEnv*, jclass)
{
    ouzel::Event event;
    event.type = ouzel::Event::Type::LOW_MEMORY;

    engine->getEventDispatcher().postEvent(event);
}

extern "C" JNIEXPORT void JNICALL Java_org_ouzelengine_OuzelLibJNIWrapper_onBackPressed(JNIEnv*, jclass)
{
    ouzel::input::InputSystemAndroid* inputSystemAndroid = static_cast<ouzel::input::InputSystemAndroid*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::KeyboardDevice* keyboardDevice = inputSystemAndroid->getKeyboardDevice();
    keyboardDevice->handleKeyPress(ouzel::input::Keyboard::Key::MENU);
}

extern "C" JNIEXPORT void JNICALL Java_org_ouzelengine_OuzelLibJNIWrapper_onKeyDown(JNIEnv*, jclass, jint keyCode)
{
    ouzel::input::InputSystemAndroid* inputSystemAndroid = static_cast<ouzel::input::InputSystemAndroid*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::KeyboardDevice* keyboardDevice = inputSystemAndroid->getKeyboardDevice();
    keyboardDevice->handleKeyPress(ouzel::input::InputSystemAndroid::convertKeyCode(keyCode));
}

extern "C" JNIEXPORT void JNICALL Java_org_ouzelengine_OuzelLibJNIWrapper_onKeyUp(JNIEnv*, jclass, jint keyCode)
{
    ouzel::input::InputSystemAndroid* inputSystemAndroid = static_cast<ouzel::input::InputSystemAndroid*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::KeyboardDevice* keyboardDevice = inputSystemAndroid->getKeyboardDevice();
    keyboardDevice->handleKeyRelease(ouzel::input::InputSystemAndroid::convertKeyCode(keyCode));
}

extern "C" JNIEXPORT jboolean JNICALL Java_org_ouzelengine_OuzelLibJNIWrapper_onTouchEvent(JNIEnv*, jclass, jobject event)
{
    ouzel::input::InputSystemAndroid* inputSystemAndroid = static_cast<ouzel::input::InputSystemAndroid*>(ouzel::engine->getInputManager()->getInputSystem());
    return inputSystemAndroid->handleTouchEvent(event);
}
