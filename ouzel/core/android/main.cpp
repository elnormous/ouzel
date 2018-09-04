// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <memory>
#include <jni.h>
#include <android/native_activity.h>
#include "core/android/EngineAndroid.hpp"
#include "core/android/NativeWindowAndroid.hpp"
#include "input/android/InputManagerAndroid.hpp"

static std::unique_ptr<ouzel::EngineAndroid> engine;

static void onDestroy(ANativeActivity* activity)
{
    engine.reset();
}

static void onStart(ANativeActivity* activity)
{
}

static void onResume(ANativeActivity* activity)
{
    engine->resume();
}

static void* onSaveInstanceState(ANativeActivity* activity, size_t* outLen)
{
    void* savedState = NULL;

    return savedState;
}

static void onPause(ANativeActivity* activity)
{
    engine->pause();
}

static void onStop(ANativeActivity* activity)
{
}

static void onConfigurationChanged(ANativeActivity* activity)
{
    engine->handleConfigurationChange();
}

static void onLowMemory(ANativeActivity* activity)
{
    engine->handleLowMemory();
}

static void onWindowFocusChanged(ANativeActivity* activity, int focused)
{
}

static void onNativeWindowCreated(ANativeActivity* activity, ANativeWindow* window)
{
    engine->handleWindowCreate(window);
    //if (!engine->isActive()) engine->run();
}

static void onNativeWindowDestroyed(ANativeActivity* activity, ANativeWindow* window)
{
    engine->handleWindowDestroy();
}

static void onInputQueueCreated(ANativeActivity* activity, AInputQueue* queue)
{
    engine->handleInputQueueCreate(queue);
}

static void onInputQueueDestroyed(ANativeActivity* activity, AInputQueue* queue)
{
    engine->handleInputQueueDestroy(queue);
}

extern "C" JNIEXPORT void ANativeActivity_onCreate(ANativeActivity* activity,
                                                   void* savedState, size_t savedStateSize)
{
    activity->callbacks->onDestroy = onDestroy;
    activity->callbacks->onStart = onStart;
    activity->callbacks->onResume = onResume;
    activity->callbacks->onSaveInstanceState = onSaveInstanceState;
    activity->callbacks->onPause = onPause;
    activity->callbacks->onStop = onStop;
    activity->callbacks->onConfigurationChanged = onConfigurationChanged;
    activity->callbacks->onLowMemory = onLowMemory;
    activity->callbacks->onWindowFocusChanged = onWindowFocusChanged;
    activity->callbacks->onNativeWindowCreated = onNativeWindowCreated;
    activity->callbacks->onNativeWindowDestroyed = onNativeWindowDestroyed;
    activity->callbacks->onInputQueueCreated = onInputQueueCreated;
    activity->callbacks->onInputQueueDestroyed = onInputQueueDestroyed;

    engine.reset(new ouzel::EngineAndroid(activity->vm));
    activity->instance = engine.get();
}

extern "C" JNIEXPORT void JNICALL Java_org_ouzelengine_OuzelLibJNIWrapper_onBackPressed(JNIEnv*, jclass)
{
    engine->getInputManager()->keyPress(ouzel::input::KeyboardKey::MENU, 0);
}

extern "C" JNIEXPORT void JNICALL Java_org_ouzelengine_OuzelLibJNIWrapper_onKeyDown(JNIEnv*, jclass, jint keyCode)
{
    engine->getInputManager()->keyPress(ouzel::input::InputManagerAndroid::convertKeyCode(keyCode), 0);
}

extern "C" JNIEXPORT void JNICALL Java_org_ouzelengine_OuzelLibJNIWrapper_onKeyUp(JNIEnv*, jclass, jint keyCode)
{
    engine->getInputManager()->keyRelease(ouzel::input::InputManagerAndroid::convertKeyCode(keyCode), 0);
}

extern "C" JNIEXPORT jboolean JNICALL Java_org_ouzelengine_OuzelLibJNIWrapper_onTouchEvent(JNIEnv*, jclass, jobject event)
{
    ouzel::input::InputManagerAndroid* inputAndroid = static_cast<ouzel::input::InputManagerAndroid*>(engine->getInputManager());
    return inputAndroid->handleTouchEvent(event);
}
