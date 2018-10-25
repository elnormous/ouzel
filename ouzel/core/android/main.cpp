// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <unordered_map>
#include <memory>
#include <jni.h>
#include <android/keycodes.h>
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
    std::unique_ptr<ouzel::SystemEvent> event(new ouzel::SystemEvent());
    event->type = ouzel::Event::Type::LOW_MEMORY;
    engine->getEventDispatcher().postEvent(std::move(event));
}

static const std::unordered_map<jint, ouzel::input::Keyboard::Key> keyMap = {
    {AKEYCODE_0, ouzel::input::Keyboard::Key::NUM_0},
    {AKEYCODE_1, ouzel::input::Keyboard::Key::NUM_1},
    {AKEYCODE_2, ouzel::input::Keyboard::Key::NUM_2},
    {AKEYCODE_3, ouzel::input::Keyboard::Key::NUM_3},
    {AKEYCODE_4, ouzel::input::Keyboard::Key::NUM_4},
    {AKEYCODE_5, ouzel::input::Keyboard::Key::NUM_5},
    {AKEYCODE_6, ouzel::input::Keyboard::Key::NUM_6},
    {AKEYCODE_7, ouzel::input::Keyboard::Key::NUM_7},
    {AKEYCODE_8, ouzel::input::Keyboard::Key::NUM_8},
    {AKEYCODE_9, ouzel::input::Keyboard::Key::NUM_9},

    {AKEYCODE_DPAD_UP, ouzel::input::Keyboard::Key::UP},
    {AKEYCODE_DPAD_DOWN, ouzel::input::Keyboard::Key::DOWN},
    {AKEYCODE_DPAD_LEFT, ouzel::input::Keyboard::Key::LEFT},
    {AKEYCODE_DPAD_RIGHT, ouzel::input::Keyboard::Key::RIGHT},
    {AKEYCODE_DPAD_CENTER, ouzel::input::Keyboard::Key::SELECT},

    {AKEYCODE_A, ouzel::input::Keyboard::Key::A},
    {AKEYCODE_B, ouzel::input::Keyboard::Key::B},
    {AKEYCODE_C, ouzel::input::Keyboard::Key::C},
    {AKEYCODE_D, ouzel::input::Keyboard::Key::D},
    {AKEYCODE_E, ouzel::input::Keyboard::Key::E},
    {AKEYCODE_F, ouzel::input::Keyboard::Key::F},
    {AKEYCODE_G, ouzel::input::Keyboard::Key::G},
    {AKEYCODE_H, ouzel::input::Keyboard::Key::H},
    {AKEYCODE_I, ouzel::input::Keyboard::Key::I},
    {AKEYCODE_J, ouzel::input::Keyboard::Key::J},
    {AKEYCODE_K, ouzel::input::Keyboard::Key::K},
    {AKEYCODE_L, ouzel::input::Keyboard::Key::L},
    {AKEYCODE_M, ouzel::input::Keyboard::Key::M},
    {AKEYCODE_N, ouzel::input::Keyboard::Key::N},
    {AKEYCODE_O, ouzel::input::Keyboard::Key::O},
    {AKEYCODE_P, ouzel::input::Keyboard::Key::P},
    {AKEYCODE_Q, ouzel::input::Keyboard::Key::Q},
    {AKEYCODE_R, ouzel::input::Keyboard::Key::R},
    {AKEYCODE_S, ouzel::input::Keyboard::Key::S},
    {AKEYCODE_T, ouzel::input::Keyboard::Key::T},
    {AKEYCODE_U, ouzel::input::Keyboard::Key::U},
    {AKEYCODE_V, ouzel::input::Keyboard::Key::V},
    {AKEYCODE_W, ouzel::input::Keyboard::Key::W},
    {AKEYCODE_X, ouzel::input::Keyboard::Key::X},
    {AKEYCODE_Y, ouzel::input::Keyboard::Key::Y},
    {AKEYCODE_Z, ouzel::input::Keyboard::Key::Z},

    {AKEYCODE_SEMICOLON, ouzel::input::Keyboard::Key::SEMICOLON},
    {AKEYCODE_EQUALS, ouzel::input::Keyboard::Key::EQUAL},
    {AKEYCODE_COMMA, ouzel::input::Keyboard::Key::COMMA},
    {AKEYCODE_PERIOD, ouzel::input::Keyboard::Key::PERIOD},
    {AKEYCODE_SLASH, ouzel::input::Keyboard::Key::SLASH},
    {AKEYCODE_GRAVE, ouzel::input::Keyboard::Key::GRAVE},
    {AKEYCODE_LEFT_BRACKET, ouzel::input::Keyboard::Key::LEFT_BRACKET},
    {AKEYCODE_BACKSLASH, ouzel::input::Keyboard::Key::BACKSLASH},
    {AKEYCODE_RIGHT_BRACKET, ouzel::input::Keyboard::Key::RIGHT_BRACKET},
    {AKEYCODE_APOSTROPHE, ouzel::input::Keyboard::Key::QUOTE},
    {AKEYCODE_SPACE, ouzel::input::Keyboard::Key::SPACE},

    {AKEYCODE_DEL, ouzel::input::Keyboard::Key::BACKSPACE},
    {AKEYCODE_TAB, ouzel::input::Keyboard::Key::TAB},
    {AKEYCODE_ENTER, ouzel::input::Keyboard::Key::ENTER},
    {AKEYCODE_BREAK, ouzel::input::Keyboard::Key::PAUSE},

    {AKEYCODE_BACK, ouzel::input::Keyboard::Key::ESCAPE},
    {AKEYCODE_PAGE_UP, ouzel::input::Keyboard::Key::PAGE_UP},
    {AKEYCODE_PAGE_DOWN, ouzel::input::Keyboard::Key::PAGE_DOWN},

    {AKEYCODE_FORWARD_DEL, ouzel::input::Keyboard::Key::DEL},

    {AKEYCODE_STAR, ouzel::input::Keyboard::Key::NONE},
    {AKEYCODE_PLUS, ouzel::input::Keyboard::Key::NONE},
    {AKEYCODE_MINUS, ouzel::input::Keyboard::Key::MINUS},

    {AKEYCODE_F1, ouzel::input::Keyboard::Key::F1},
    {AKEYCODE_F2, ouzel::input::Keyboard::Key::F2},
    {AKEYCODE_F3, ouzel::input::Keyboard::Key::F3},
    {AKEYCODE_F4, ouzel::input::Keyboard::Key::F4},
    {AKEYCODE_F5, ouzel::input::Keyboard::Key::F5},
    {AKEYCODE_F6, ouzel::input::Keyboard::Key::F6},
    {AKEYCODE_F7, ouzel::input::Keyboard::Key::F7},
    {AKEYCODE_F8, ouzel::input::Keyboard::Key::F8},
    {AKEYCODE_F9, ouzel::input::Keyboard::Key::F9},
    {AKEYCODE_F10, ouzel::input::Keyboard::Key::F10},
    {AKEYCODE_F11, ouzel::input::Keyboard::Key::F11},
    {AKEYCODE_F12, ouzel::input::Keyboard::Key::F12},

    {AKEYCODE_NUMPAD_0, ouzel::input::Keyboard::Key::NUMPAD_0},
    {AKEYCODE_NUMPAD_1, ouzel::input::Keyboard::Key::NUMPAD_1},
    {AKEYCODE_NUMPAD_2, ouzel::input::Keyboard::Key::NUMPAD_2},
    {AKEYCODE_NUMPAD_3, ouzel::input::Keyboard::Key::NUMPAD_3},
    {AKEYCODE_NUMPAD_4, ouzel::input::Keyboard::Key::NUMPAD_4},
    {AKEYCODE_NUMPAD_5, ouzel::input::Keyboard::Key::NUMPAD_5},
    {AKEYCODE_NUMPAD_6, ouzel::input::Keyboard::Key::NUMPAD_6},
    {AKEYCODE_NUMPAD_7, ouzel::input::Keyboard::Key::NUMPAD_7},
    {AKEYCODE_NUMPAD_8, ouzel::input::Keyboard::Key::NUMPAD_8},
    {AKEYCODE_NUMPAD_9, ouzel::input::Keyboard::Key::NUMPAD_9},

    {AKEYCODE_NUMPAD_DIVIDE, ouzel::input::Keyboard::Key::NUMPAD_DIVIDE},
    {AKEYCODE_NUMPAD_MULTIPLY, ouzel::input::Keyboard::Key::NUMPAD_MULTIPLY},
    {AKEYCODE_NUMPAD_SUBTRACT, ouzel::input::Keyboard::Key::NUMPAD_MINUS},
    {AKEYCODE_NUMPAD_ADD, ouzel::input::Keyboard::Key::NUMPAD_PLUS},
    {AKEYCODE_NUMPAD_DOT, ouzel::input::Keyboard::Key::NUMPAD_DECIMAL},
    {AKEYCODE_NUMPAD_COMMA, ouzel::input::Keyboard::Key::NUMPAD_SEPARATOR},
    {AKEYCODE_NUMPAD_ENTER, ouzel::input::Keyboard::Key::NUMPAD_ENTER},
    {AKEYCODE_NUMPAD_EQUALS, ouzel::input::Keyboard::Key::NUMPAD_EQUAL},
    {AKEYCODE_NUMPAD_LEFT_PAREN, ouzel::input::Keyboard::Key::NUMPAD_LEFT_PAREN},
    {AKEYCODE_NUMPAD_RIGHT_PAREN, ouzel::input::Keyboard::Key::NUMPAD_RIGHT_PAREN},

    {AKEYCODE_SHIFT_LEFT, ouzel::input::Keyboard::Key::LEFT_SHIFT},
    {AKEYCODE_SHIFT_RIGHT, ouzel::input::Keyboard::Key::RIGHT_SHIFT},
    {AKEYCODE_CTRL_LEFT, ouzel::input::Keyboard::Key::LEFT_CONTROL},
    {AKEYCODE_CTRL_RIGHT, ouzel::input::Keyboard::Key::RIGHT_CONTROL},
    {AKEYCODE_ALT_LEFT, ouzel::input::Keyboard::Key::LEFT_ALT},
    {AKEYCODE_ALT_RIGHT, ouzel::input::Keyboard::Key::RIGHT_ALT},
    {AKEYCODE_META_LEFT, ouzel::input::Keyboard::Key::LEFT_SUPER},
    {AKEYCODE_META_RIGHT, ouzel::input::Keyboard::Key::RIGHT_SUPER},
    {AKEYCODE_MENU, ouzel::input::Keyboard::Key::MENU},
    {AKEYCODE_CLEAR, ouzel::input::Keyboard::Key::CLEAR},
    {AKEYCODE_ESCAPE, ouzel::input::Keyboard::Key::ESCAPE},
    {AKEYCODE_CAPS_LOCK, ouzel::input::Keyboard::Key::CAPS_LOCK},
    {AKEYCODE_SCROLL_LOCK, ouzel::input::Keyboard::Key::SCROLL_LOCK},
    {AKEYCODE_SYSRQ, ouzel::input::Keyboard::Key::PRINT_SCREEN},
    {AKEYCODE_MOVE_HOME, ouzel::input::Keyboard::Key::HOME},
    {AKEYCODE_MOVE_END, ouzel::input::Keyboard::Key::END},
    {AKEYCODE_INSERT, ouzel::input::Keyboard::Key::INSERT},
    {AKEYCODE_SLEEP, ouzel::input::Keyboard::Key::SLEEP},
    {AKEYCODE_POWER, ouzel::input::Keyboard::Key::POWER},
    {AKEYCODE_HELP, ouzel::input::Keyboard::Key::HELP},
    {AKEYCODE_YEN, ouzel::input::Keyboard::Key::YEN},
    {AKEYCODE_RO, ouzel::input::Keyboard::Key::RO},
    {AKEYCODE_KANA, ouzel::input::Keyboard::Key::KANA},
    {AKEYCODE_KATAKANA_HIRAGANA, ouzel::input::Keyboard::Key::KATAKANA_HIRAGANA},
    {AKEYCODE_MUHENKAN, ouzel::input::Keyboard::Key::MUHENKAN},
    {AKEYCODE_HENKAN, ouzel::input::Keyboard::Key::HENKAN},
    
    {AKEYCODE_HOME, ouzel::input::Keyboard::Key::HOME},
    {AKEYCODE_BACK, ouzel::input::Keyboard::Key::BACK},
    {AKEYCODE_FORWARD, ouzel::input::Keyboard::Key::FORWARD},
    {AKEYCODE_SEARCH, ouzel::input::Keyboard::Key::SEARCH},
    {AKEYCODE_BOOKMARK, ouzel::input::Keyboard::Key::BOOKMARKS},
    {AKEYCODE_VOLUME_MUTE, ouzel::input::Keyboard::Key::MUTE},
    {AKEYCODE_VOLUME_UP, ouzel::input::Keyboard::Key::VOLUME_UP},
    {AKEYCODE_VOLUME_DOWN, ouzel::input::Keyboard::Key::VOLUME_DOWN},
    {AKEYCODE_MEDIA_PLAY, ouzel::input::Keyboard::Key::AUDIO_PLAY},
    {AKEYCODE_MEDIA_PLAY_PAUSE, ouzel::input::Keyboard::Key::AUDIO_PLAY},
    {AKEYCODE_MEDIA_STOP, ouzel::input::Keyboard::Key::AUDIO_STOP},
    {AKEYCODE_MEDIA_NEXT, ouzel::input::Keyboard::Key::AUDIO_NEXT},
    {AKEYCODE_MEDIA_PREVIOUS, ouzel::input::Keyboard::Key::AUDIO_PREVIOUS},
    {AKEYCODE_ENVELOPE, ouzel::input::Keyboard::Key::MAIL}
    
};

static ouzel::input::Keyboard::Key convertKeyCode(jint keyCode)
{
    auto i = keyMap.find(keyCode);

    if (i != keyMap.end())
        return i->second;
    else
        return ouzel::input::Keyboard::Key::NONE;
}

extern "C" JNIEXPORT jboolean JNICALL Java_org_ouzelengine_OuzelLibJNIWrapper_onKeyDown(JNIEnv*, jclass, jint keyCode)
{
    ouzel::input::InputSystemAndroid* inputSystemAndroid = static_cast<ouzel::input::InputSystemAndroid*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::KeyboardDevice* keyboardDevice = inputSystemAndroid->getKeyboardDevice();
    std::future<bool> f = keyboardDevice->handleKeyPress(convertKeyCode(keyCode));

    if (keyCode == AKEYCODE_BACK)
        return f.get();
    else
        return true;
}

extern "C" JNIEXPORT jboolean JNICALL Java_org_ouzelengine_OuzelLibJNIWrapper_onKeyUp(JNIEnv*, jclass, jint keyCode)
{
    ouzel::input::InputSystemAndroid* inputSystemAndroid = static_cast<ouzel::input::InputSystemAndroid*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::KeyboardDevice* keyboardDevice = inputSystemAndroid->getKeyboardDevice();
    std::future<bool> f = keyboardDevice->handleKeyRelease(convertKeyCode(keyCode));

    if (keyCode == AKEYCODE_BACK)
        return f.get();
    else
        return true;
}

extern "C" JNIEXPORT jboolean JNICALL Java_org_ouzelengine_OuzelLibJNIWrapper_onTouchEvent(JNIEnv*, jclass, jobject event)
{
    ouzel::input::InputSystemAndroid* inputSystemAndroid = static_cast<ouzel::input::InputSystemAndroid*>(ouzel::engine->getInputManager()->getInputSystem());
    return inputSystemAndroid->handleTouchEvent(event);
}
