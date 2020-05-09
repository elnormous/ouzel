// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include <unordered_map>
#include <memory>
#include <jni.h>
#include <android/keycodes.h>
#include "SystemAndroid.hpp"
#include "EngineAndroid.hpp"
#include "NativeWindowAndroid.hpp"
#include "../Engine.hpp"
#include "../../events/EventDispatcher.hpp"
#include "../../input/android/InputSystemAndroid.hpp"
#include "../../utils/Log.hpp"

namespace
{
    std::unique_ptr<ouzel::EngineAndroid> engine;

    const std::unordered_map<jint, ouzel::input::Keyboard::Key> keyMap = {
        {AKEYCODE_0, ouzel::input::Keyboard::Key::Num0},
        {AKEYCODE_1, ouzel::input::Keyboard::Key::Num1},
        {AKEYCODE_2, ouzel::input::Keyboard::Key::Num2},
        {AKEYCODE_3, ouzel::input::Keyboard::Key::Num3},
        {AKEYCODE_4, ouzel::input::Keyboard::Key::Num4},
        {AKEYCODE_5, ouzel::input::Keyboard::Key::Num5},
        {AKEYCODE_6, ouzel::input::Keyboard::Key::Num6},
        {AKEYCODE_7, ouzel::input::Keyboard::Key::Num7},
        {AKEYCODE_8, ouzel::input::Keyboard::Key::Num8},
        {AKEYCODE_9, ouzel::input::Keyboard::Key::Num9},

        {AKEYCODE_DPAD_UP, ouzel::input::Keyboard::Key::Up},
        {AKEYCODE_DPAD_DOWN, ouzel::input::Keyboard::Key::Down},
        {AKEYCODE_DPAD_LEFT, ouzel::input::Keyboard::Key::Left},
        {AKEYCODE_DPAD_RIGHT, ouzel::input::Keyboard::Key::Right},
        {AKEYCODE_DPAD_CENTER, ouzel::input::Keyboard::Key::Select},

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

        {AKEYCODE_SEMICOLON, ouzel::input::Keyboard::Key::Semicolon},
        {AKEYCODE_EQUALS, ouzel::input::Keyboard::Key::Equal},
        {AKEYCODE_COMMA, ouzel::input::Keyboard::Key::Comma},
        {AKEYCODE_PERIOD, ouzel::input::Keyboard::Key::Period},
        {AKEYCODE_SLASH, ouzel::input::Keyboard::Key::Slash},
        {AKEYCODE_GRAVE, ouzel::input::Keyboard::Key::Grave},
        {AKEYCODE_LEFT_BRACKET, ouzel::input::Keyboard::Key::LeftBracket},
        {AKEYCODE_BACKSLASH, ouzel::input::Keyboard::Key::Backslash},
        {AKEYCODE_RIGHT_BRACKET, ouzel::input::Keyboard::Key::RightBracket},
        {AKEYCODE_APOSTROPHE, ouzel::input::Keyboard::Key::Quote},
        {AKEYCODE_SPACE, ouzel::input::Keyboard::Key::Space},

        {AKEYCODE_DEL, ouzel::input::Keyboard::Key::Backspace},
        {AKEYCODE_TAB, ouzel::input::Keyboard::Key::Tab},
        {AKEYCODE_ENTER, ouzel::input::Keyboard::Key::Enter},
        {AKEYCODE_BREAK, ouzel::input::Keyboard::Key::Pause},

        {AKEYCODE_PAGE_UP, ouzel::input::Keyboard::Key::PageUp},
        {AKEYCODE_PAGE_DOWN, ouzel::input::Keyboard::Key::PageDown},

        {AKEYCODE_FORWARD_DEL, ouzel::input::Keyboard::Key::Del},

        {AKEYCODE_STAR, ouzel::input::Keyboard::Key::Unknown},
        {AKEYCODE_PLUS, ouzel::input::Keyboard::Key::Unknown},
        {AKEYCODE_MINUS, ouzel::input::Keyboard::Key::Minus},

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

        {AKEYCODE_NUMPAD_0, ouzel::input::Keyboard::Key::Numpad0},
        {AKEYCODE_NUMPAD_1, ouzel::input::Keyboard::Key::Numpad1},
        {AKEYCODE_NUMPAD_2, ouzel::input::Keyboard::Key::Numpad2},
        {AKEYCODE_NUMPAD_3, ouzel::input::Keyboard::Key::Numpad3},
        {AKEYCODE_NUMPAD_4, ouzel::input::Keyboard::Key::Numpad4},
        {AKEYCODE_NUMPAD_5, ouzel::input::Keyboard::Key::Numpad5},
        {AKEYCODE_NUMPAD_6, ouzel::input::Keyboard::Key::Numpad6},
        {AKEYCODE_NUMPAD_7, ouzel::input::Keyboard::Key::Numpad7},
        {AKEYCODE_NUMPAD_8, ouzel::input::Keyboard::Key::Numpad8},
        {AKEYCODE_NUMPAD_9, ouzel::input::Keyboard::Key::Numpad9},

        {AKEYCODE_NUMPAD_DIVIDE, ouzel::input::Keyboard::Key::NumpadDivide},
        {AKEYCODE_NUMPAD_MULTIPLY, ouzel::input::Keyboard::Key::NumpadMultiply},
        {AKEYCODE_NUMPAD_SUBTRACT, ouzel::input::Keyboard::Key::NumpadMinus},
        {AKEYCODE_NUMPAD_ADD, ouzel::input::Keyboard::Key::NumpadPlus},
        {AKEYCODE_NUMPAD_DOT, ouzel::input::Keyboard::Key::NumpadDecimal},
        {AKEYCODE_NUMPAD_COMMA, ouzel::input::Keyboard::Key::NumpadSeparator},
        {AKEYCODE_NUMPAD_ENTER, ouzel::input::Keyboard::Key::NumpadEnter},
        {AKEYCODE_NUMPAD_EQUALS, ouzel::input::Keyboard::Key::NumpadEqual},
        {AKEYCODE_NUMPAD_LEFT_PAREN, ouzel::input::Keyboard::Key::NumpadLeftParen},
        {AKEYCODE_NUMPAD_RIGHT_PAREN, ouzel::input::Keyboard::Key::NumpadRightParen},

        {AKEYCODE_SHIFT_LEFT, ouzel::input::Keyboard::Key::LeftShift},
        {AKEYCODE_SHIFT_RIGHT, ouzel::input::Keyboard::Key::RightShift},
        {AKEYCODE_CTRL_LEFT, ouzel::input::Keyboard::Key::LeftControl},
        {AKEYCODE_CTRL_RIGHT, ouzel::input::Keyboard::Key::RightControl},
        {AKEYCODE_ALT_LEFT, ouzel::input::Keyboard::Key::LeftAlt},
        {AKEYCODE_ALT_RIGHT, ouzel::input::Keyboard::Key::RightAlt},
        {AKEYCODE_META_LEFT, ouzel::input::Keyboard::Key::LeftSuper},
        {AKEYCODE_META_RIGHT, ouzel::input::Keyboard::Key::RightSuper},
        {AKEYCODE_MENU, ouzel::input::Keyboard::Key::Menu},
        {AKEYCODE_CLEAR, ouzel::input::Keyboard::Key::Clear},
        {AKEYCODE_ESCAPE, ouzel::input::Keyboard::Key::Escape},
        {AKEYCODE_CAPS_LOCK, ouzel::input::Keyboard::Key::CapsLock},
        {AKEYCODE_SCROLL_LOCK, ouzel::input::Keyboard::Key::ScrollLock},
        {AKEYCODE_SYSRQ, ouzel::input::Keyboard::Key::PrintScreen},
        {AKEYCODE_MOVE_HOME, ouzel::input::Keyboard::Key::Home},
        {AKEYCODE_MOVE_END, ouzel::input::Keyboard::Key::End},
        {AKEYCODE_INSERT, ouzel::input::Keyboard::Key::Insert},
        {AKEYCODE_SLEEP, ouzel::input::Keyboard::Key::Sleep},
        {AKEYCODE_POWER, ouzel::input::Keyboard::Key::Power},
        {AKEYCODE_HELP, ouzel::input::Keyboard::Key::Help},
        {AKEYCODE_YEN, ouzel::input::Keyboard::Key::Yen},
        {AKEYCODE_RO, ouzel::input::Keyboard::Key::Ro},
        {AKEYCODE_KANA, ouzel::input::Keyboard::Key::Kana},
        {AKEYCODE_KATAKANA_HIRAGANA, ouzel::input::Keyboard::Key::KatakanaHiragana},
        {AKEYCODE_MUHENKAN, ouzel::input::Keyboard::Key::Muhenkan},
        {AKEYCODE_HENKAN, ouzel::input::Keyboard::Key::Henkan},

        {AKEYCODE_HOME, ouzel::input::Keyboard::Key::Home},
        {AKEYCODE_BACK, ouzel::input::Keyboard::Key::Back},
        {AKEYCODE_FORWARD, ouzel::input::Keyboard::Key::Forward},
        {AKEYCODE_SEARCH, ouzel::input::Keyboard::Key::Search},
        {AKEYCODE_BOOKMARK, ouzel::input::Keyboard::Key::Bookmarks},
        {AKEYCODE_VOLUME_MUTE, ouzel::input::Keyboard::Key::Mute},
        {AKEYCODE_VOLUME_UP, ouzel::input::Keyboard::Key::VolumeUp},
        {AKEYCODE_VOLUME_DOWN, ouzel::input::Keyboard::Key::VolumeDown},
        {AKEYCODE_MEDIA_PLAY, ouzel::input::Keyboard::Key::AudioPlay},
        {AKEYCODE_MEDIA_PLAY_PAUSE, ouzel::input::Keyboard::Key::AudioPlay},
        {AKEYCODE_MEDIA_STOP, ouzel::input::Keyboard::Key::AudioStop},
        {AKEYCODE_MEDIA_NEXT, ouzel::input::Keyboard::Key::AudioNext},
        {AKEYCODE_MEDIA_PREVIOUS, ouzel::input::Keyboard::Key::AudioPrevious},
        {AKEYCODE_ENVELOPE, ouzel::input::Keyboard::Key::Mail}
    };

    ouzel::input::Keyboard::Key convertKeyCode(jint keyCode)
    {
        auto i = keyMap.find(keyCode);

        if (i != keyMap.end())
            return i->second;
        else
            return ouzel::input::Keyboard::Key::Unknown;
    }
}

extern "C" JNIEXPORT jint JNIEXPORT JNI_OnLoad(JavaVM* javaVm, void*)
{
    engine = std::make_unique<ouzel::EngineAndroid>(javaVm);
    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNIEXPORT JNI_OnUnload(JavaVM*, void*)
{
    engine->exit();
    engine.reset();
}

extern "C" JNIEXPORT void JNICALL Java_org_ouzel_OuzelLibJNIWrapper_onCreate(JNIEnv*, jclass, jobject mainActivity)
{
    engine->onCreate(mainActivity);
}

extern "C" JNIEXPORT void JNICALL Java_org_ouzel_OuzelLibJNIWrapper_onSurfaceCreated(JNIEnv*, jclass, jobject surface)
{
    try
    {
        engine->onSurfaceCreated(surface);

        if (!engine->isActive()) engine->run();
    }
    catch (const std::exception& e)
    {
        ouzel::engine->log(ouzel::Log::Level::error) << e.what();
    }
}

extern "C" JNIEXPORT void JNICALL Java_org_ouzel_OuzelLibJNIWrapper_onSurfaceDestroyed(JNIEnv*, jclass)
{
    engine->onSurfaceDestroyed();
}

extern "C" JNIEXPORT void JNICALL Java_org_ouzel_OuzelLibJNIWrapper_onSurfaceChanged(JNIEnv*, jclass, jobject, jint width, jint height)
{
    ouzel::NativeWindowAndroid* windowAndroid = static_cast<ouzel::NativeWindowAndroid*>(engine->getWindow()->getNativeWindow());
    windowAndroid->handleResize(ouzel::Size2U(static_cast<std::uint32_t>(width),
                                              static_cast<std::uint32_t>(height)));
}

extern "C" JNIEXPORT void JNICALL Java_org_ouzel_OuzelLibJNIWrapper_onStart(JNIEnv*, jclass)
{
    // Do nothing
}

extern "C" JNIEXPORT void JNICALL Java_org_ouzel_OuzelLibJNIWrapper_onPause(JNIEnv*, jclass)
{
    engine->pause();
}

extern "C" JNIEXPORT void JNICALL Java_org_ouzel_OuzelLibJNIWrapper_onResume(JNIEnv*, jclass)
{
    engine->resume();
}

extern "C" JNIEXPORT void JNICALL Java_org_ouzel_OuzelLibJNIWrapper_onConfigurationChanged(JNIEnv*, jclass, jobject newConfig)
{
    engine->onConfigurationChanged(newConfig);
}

extern "C" JNIEXPORT void JNICALL Java_org_ouzel_OuzelLibJNIWrapper_onLowMemory(JNIEnv*, jclass)
{
    auto event = std::make_unique<ouzel::SystemEvent>();
    event->type = ouzel::Event::Type::LowMemory;
    engine->getEventDispatcher().postEvent(std::move(event));
}

extern "C" JNIEXPORT jboolean JNICALL Java_org_ouzel_OuzelLibJNIWrapper_onKeyDown(JNIEnv*, jclass, jint keyCode)
{
    ouzel::input::InputSystemAndroid* inputSystemAndroid = static_cast<ouzel::input::InputSystemAndroid*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::KeyboardDevice* keyboardDevice = inputSystemAndroid->getKeyboardDevice();
    std::future<bool> f = keyboardDevice->handleKeyPress(convertKeyCode(keyCode));

    if (keyCode == AKEYCODE_BACK)
        return f.get();
    else
        return true;
}

extern "C" JNIEXPORT jboolean JNICALL Java_org_ouzel_OuzelLibJNIWrapper_onKeyUp(JNIEnv*, jclass, jint keyCode)
{
    ouzel::input::InputSystemAndroid* inputSystemAndroid = static_cast<ouzel::input::InputSystemAndroid*>(ouzel::engine->getInputManager()->getInputSystem());
    ouzel::input::KeyboardDevice* keyboardDevice = inputSystemAndroid->getKeyboardDevice();
    std::future<bool> f = keyboardDevice->handleKeyRelease(convertKeyCode(keyCode));

    if (keyCode == AKEYCODE_BACK)
        return f.get();
    else
        return true;
}

extern "C" JNIEXPORT jboolean JNICALL Java_org_ouzel_OuzelLibJNIWrapper_onTouchEvent(JNIEnv*, jclass, jobject event)
{
    ouzel::input::InputSystemAndroid* inputSystemAndroid = static_cast<ouzel::input::InputSystemAndroid*>(ouzel::engine->getInputManager()->getInputSystem());
    return inputSystemAndroid->handleTouchEvent(event);
}

extern "C" JNIEXPORT jboolean JNICALL Java_org_ouzel_OuzelLibJNIWrapper_onGenericMotionEvent(JNIEnv*, jclass, jobject event)
{
    ouzel::input::InputSystemAndroid* inputSystemAndroid = static_cast<ouzel::input::InputSystemAndroid*>(ouzel::engine->getInputManager()->getInputSystem());
    return inputSystemAndroid->handleGenericMotionEvent(event);
}

namespace ouzel
{
}
