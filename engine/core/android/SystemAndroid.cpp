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
    ouzel::core::android::System system;
    std::unique_ptr<ouzel::core::android::Engine> engine;

    const std::unordered_map<jint, ouzel::input::Keyboard::Key> keyMap = {
        {AKEYCODE_0, ouzel::input::Keyboard::Key::num0},
        {AKEYCODE_1, ouzel::input::Keyboard::Key::num1},
        {AKEYCODE_2, ouzel::input::Keyboard::Key::num2},
        {AKEYCODE_3, ouzel::input::Keyboard::Key::num3},
        {AKEYCODE_4, ouzel::input::Keyboard::Key::num4},
        {AKEYCODE_5, ouzel::input::Keyboard::Key::num5},
        {AKEYCODE_6, ouzel::input::Keyboard::Key::num6},
        {AKEYCODE_7, ouzel::input::Keyboard::Key::num7},
        {AKEYCODE_8, ouzel::input::Keyboard::Key::num8},
        {AKEYCODE_9, ouzel::input::Keyboard::Key::num9},

        {AKEYCODE_DPAD_UP, ouzel::input::Keyboard::Key::up},
        {AKEYCODE_DPAD_DOWN, ouzel::input::Keyboard::Key::down},
        {AKEYCODE_DPAD_LEFT, ouzel::input::Keyboard::Key::left},
        {AKEYCODE_DPAD_RIGHT, ouzel::input::Keyboard::Key::right},
        {AKEYCODE_DPAD_CENTER, ouzel::input::Keyboard::Key::select},

        {AKEYCODE_A, ouzel::input::Keyboard::Key::a},
        {AKEYCODE_B, ouzel::input::Keyboard::Key::b},
        {AKEYCODE_C, ouzel::input::Keyboard::Key::c},
        {AKEYCODE_D, ouzel::input::Keyboard::Key::d},
        {AKEYCODE_E, ouzel::input::Keyboard::Key::e},
        {AKEYCODE_F, ouzel::input::Keyboard::Key::f},
        {AKEYCODE_G, ouzel::input::Keyboard::Key::g},
        {AKEYCODE_H, ouzel::input::Keyboard::Key::h},
        {AKEYCODE_I, ouzel::input::Keyboard::Key::i},
        {AKEYCODE_J, ouzel::input::Keyboard::Key::j},
        {AKEYCODE_K, ouzel::input::Keyboard::Key::k},
        {AKEYCODE_L, ouzel::input::Keyboard::Key::l},
        {AKEYCODE_M, ouzel::input::Keyboard::Key::m},
        {AKEYCODE_N, ouzel::input::Keyboard::Key::n},
        {AKEYCODE_O, ouzel::input::Keyboard::Key::o},
        {AKEYCODE_P, ouzel::input::Keyboard::Key::p},
        {AKEYCODE_Q, ouzel::input::Keyboard::Key::q},
        {AKEYCODE_R, ouzel::input::Keyboard::Key::r},
        {AKEYCODE_S, ouzel::input::Keyboard::Key::s},
        {AKEYCODE_T, ouzel::input::Keyboard::Key::t},
        {AKEYCODE_U, ouzel::input::Keyboard::Key::u},
        {AKEYCODE_V, ouzel::input::Keyboard::Key::v},
        {AKEYCODE_W, ouzel::input::Keyboard::Key::w},
        {AKEYCODE_X, ouzel::input::Keyboard::Key::x},
        {AKEYCODE_Y, ouzel::input::Keyboard::Key::y},
        {AKEYCODE_Z, ouzel::input::Keyboard::Key::z},

        {AKEYCODE_SEMICOLON, ouzel::input::Keyboard::Key::semicolon},
        {AKEYCODE_EQUALS, ouzel::input::Keyboard::Key::equal},
        {AKEYCODE_COMMA, ouzel::input::Keyboard::Key::comma},
        {AKEYCODE_PERIOD, ouzel::input::Keyboard::Key::period},
        {AKEYCODE_SLASH, ouzel::input::Keyboard::Key::slash},
        {AKEYCODE_GRAVE, ouzel::input::Keyboard::Key::grave},
        {AKEYCODE_LEFT_BRACKET, ouzel::input::Keyboard::Key::leftBracket},
        {AKEYCODE_BACKSLASH, ouzel::input::Keyboard::Key::backslash},
        {AKEYCODE_RIGHT_BRACKET, ouzel::input::Keyboard::Key::rightBracket},
        {AKEYCODE_APOSTROPHE, ouzel::input::Keyboard::Key::quote},
        {AKEYCODE_SPACE, ouzel::input::Keyboard::Key::space},

        {AKEYCODE_DEL, ouzel::input::Keyboard::Key::backspace},
        {AKEYCODE_TAB, ouzel::input::Keyboard::Key::tab},
        {AKEYCODE_ENTER, ouzel::input::Keyboard::Key::enter},
        {AKEYCODE_BREAK, ouzel::input::Keyboard::Key::pause},

        {AKEYCODE_PAGE_UP, ouzel::input::Keyboard::Key::pageUp},
        {AKEYCODE_PAGE_DOWN, ouzel::input::Keyboard::Key::pageDown},

        {AKEYCODE_FORWARD_DEL, ouzel::input::Keyboard::Key::del},

        {AKEYCODE_STAR, ouzel::input::Keyboard::Key::none},
        {AKEYCODE_PLUS, ouzel::input::Keyboard::Key::none},
        {AKEYCODE_MINUS, ouzel::input::Keyboard::Key::minus},

        {AKEYCODE_F1, ouzel::input::Keyboard::Key::f1},
        {AKEYCODE_F2, ouzel::input::Keyboard::Key::f2},
        {AKEYCODE_F3, ouzel::input::Keyboard::Key::f3},
        {AKEYCODE_F4, ouzel::input::Keyboard::Key::f4},
        {AKEYCODE_F5, ouzel::input::Keyboard::Key::f5},
        {AKEYCODE_F6, ouzel::input::Keyboard::Key::f6},
        {AKEYCODE_F7, ouzel::input::Keyboard::Key::f7},
        {AKEYCODE_F8, ouzel::input::Keyboard::Key::f8},
        {AKEYCODE_F9, ouzel::input::Keyboard::Key::f9},
        {AKEYCODE_F10, ouzel::input::Keyboard::Key::f10},
        {AKEYCODE_F11, ouzel::input::Keyboard::Key::f11},
        {AKEYCODE_F12, ouzel::input::Keyboard::Key::f12},

        {AKEYCODE_NUMPAD_0, ouzel::input::Keyboard::Key::numpad0},
        {AKEYCODE_NUMPAD_1, ouzel::input::Keyboard::Key::numpad1},
        {AKEYCODE_NUMPAD_2, ouzel::input::Keyboard::Key::numpad2},
        {AKEYCODE_NUMPAD_3, ouzel::input::Keyboard::Key::numpad3},
        {AKEYCODE_NUMPAD_4, ouzel::input::Keyboard::Key::numpad4},
        {AKEYCODE_NUMPAD_5, ouzel::input::Keyboard::Key::numpad5},
        {AKEYCODE_NUMPAD_6, ouzel::input::Keyboard::Key::numpad6},
        {AKEYCODE_NUMPAD_7, ouzel::input::Keyboard::Key::numpad7},
        {AKEYCODE_NUMPAD_8, ouzel::input::Keyboard::Key::numpad8},
        {AKEYCODE_NUMPAD_9, ouzel::input::Keyboard::Key::numpad9},

        {AKEYCODE_NUMPAD_DIVIDE, ouzel::input::Keyboard::Key::numpadDivide},
        {AKEYCODE_NUMPAD_MULTIPLY, ouzel::input::Keyboard::Key::numpadMultiply},
        {AKEYCODE_NUMPAD_SUBTRACT, ouzel::input::Keyboard::Key::numpadMinus},
        {AKEYCODE_NUMPAD_ADD, ouzel::input::Keyboard::Key::numpadPlus},
        {AKEYCODE_NUMPAD_DOT, ouzel::input::Keyboard::Key::numpadDecimal},
        {AKEYCODE_NUMPAD_COMMA, ouzel::input::Keyboard::Key::numpadSeparator},
        {AKEYCODE_NUMPAD_ENTER, ouzel::input::Keyboard::Key::numpadEnter},
        {AKEYCODE_NUMPAD_EQUALS, ouzel::input::Keyboard::Key::numpadEqual},
        {AKEYCODE_NUMPAD_LEFT_PAREN, ouzel::input::Keyboard::Key::numpadLeftParen},
        {AKEYCODE_NUMPAD_RIGHT_PAREN, ouzel::input::Keyboard::Key::numpadRightParen},

        {AKEYCODE_SHIFT_LEFT, ouzel::input::Keyboard::Key::leftShift},
        {AKEYCODE_SHIFT_RIGHT, ouzel::input::Keyboard::Key::rightShift},
        {AKEYCODE_CTRL_LEFT, ouzel::input::Keyboard::Key::leftControl},
        {AKEYCODE_CTRL_RIGHT, ouzel::input::Keyboard::Key::rightControl},
        {AKEYCODE_ALT_LEFT, ouzel::input::Keyboard::Key::leftAlt},
        {AKEYCODE_ALT_RIGHT, ouzel::input::Keyboard::Key::rightAlt},
        {AKEYCODE_META_LEFT, ouzel::input::Keyboard::Key::leftSuper},
        {AKEYCODE_META_RIGHT, ouzel::input::Keyboard::Key::rightSuper},
        {AKEYCODE_MENU, ouzel::input::Keyboard::Key::menu},
        {AKEYCODE_CLEAR, ouzel::input::Keyboard::Key::clear},
        {AKEYCODE_ESCAPE, ouzel::input::Keyboard::Key::escape},
        {AKEYCODE_CAPS_LOCK, ouzel::input::Keyboard::Key::capsLock},
        {AKEYCODE_SCROLL_LOCK, ouzel::input::Keyboard::Key::scrollLock},
        {AKEYCODE_SYSRQ, ouzel::input::Keyboard::Key::printScreen},
        {AKEYCODE_MOVE_HOME, ouzel::input::Keyboard::Key::home},
        {AKEYCODE_MOVE_END, ouzel::input::Keyboard::Key::end},
        {AKEYCODE_INSERT, ouzel::input::Keyboard::Key::insert},
        {AKEYCODE_SLEEP, ouzel::input::Keyboard::Key::sleep},
        {AKEYCODE_POWER, ouzel::input::Keyboard::Key::power},
        {AKEYCODE_HELP, ouzel::input::Keyboard::Key::help},
        {AKEYCODE_YEN, ouzel::input::Keyboard::Key::yen},
        {AKEYCODE_RO, ouzel::input::Keyboard::Key::ro},
        {AKEYCODE_KANA, ouzel::input::Keyboard::Key::kana},
        {AKEYCODE_KATAKANA_HIRAGANA, ouzel::input::Keyboard::Key::katakanaHiragana},
        {AKEYCODE_MUHENKAN, ouzel::input::Keyboard::Key::muhenkan},
        {AKEYCODE_HENKAN, ouzel::input::Keyboard::Key::henkan},

        {AKEYCODE_HOME, ouzel::input::Keyboard::Key::home},
        {AKEYCODE_BACK, ouzel::input::Keyboard::Key::back},
        {AKEYCODE_FORWARD, ouzel::input::Keyboard::Key::forward},
        {AKEYCODE_SEARCH, ouzel::input::Keyboard::Key::search},
        {AKEYCODE_BOOKMARK, ouzel::input::Keyboard::Key::bookmarks},
        {AKEYCODE_VOLUME_MUTE, ouzel::input::Keyboard::Key::mute},
        {AKEYCODE_VOLUME_UP, ouzel::input::Keyboard::Key::volumeUp},
        {AKEYCODE_VOLUME_DOWN, ouzel::input::Keyboard::Key::volumeDown},
        {AKEYCODE_MEDIA_PLAY, ouzel::input::Keyboard::Key::audioPlay},
        {AKEYCODE_MEDIA_PLAY_PAUSE, ouzel::input::Keyboard::Key::audioPlay},
        {AKEYCODE_MEDIA_STOP, ouzel::input::Keyboard::Key::audioStop},
        {AKEYCODE_MEDIA_NEXT, ouzel::input::Keyboard::Key::audioNext},
        {AKEYCODE_MEDIA_PREVIOUS, ouzel::input::Keyboard::Key::audioPrevious},
        {AKEYCODE_ENVELOPE, ouzel::input::Keyboard::Key::mail}
    };

    ouzel::input::Keyboard::Key convertKeyCode(jint keyCode)
    {
        auto i = keyMap.find(keyCode);

        if (i != keyMap.end())
            return i->second;
        else
            return ouzel::input::Keyboard::Key::none;
    }
}

extern "C" JNIEXPORT jint JNIEXPORT JNI_OnLoad(JavaVM* javaVm, void*)
{
    engine = std::make_unique<ouzel::core::android::Engine>(javaVm);
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
        ouzel::logger.log(ouzel::Log::Level::error) << e.what();
    }
}

extern "C" JNIEXPORT void JNICALL Java_org_ouzel_OuzelLibJNIWrapper_onSurfaceDestroyed(JNIEnv*, jclass)
{
    engine->onSurfaceDestroyed();
}

extern "C" JNIEXPORT void JNICALL Java_org_ouzel_OuzelLibJNIWrapper_onSurfaceChanged(JNIEnv*, jclass, jobject, jint width, jint height)
{
    auto windowAndroid = static_cast<ouzel::core::android::NativeWindow*>(engine->getWindow()->getNativeWindow());
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
    event->type = ouzel::Event::Type::lowMemory;
    engine->getEventDispatcher().postEvent(std::move(event));
}

extern "C" JNIEXPORT jboolean JNICALL Java_org_ouzel_OuzelLibJNIWrapper_onKeyDown(JNIEnv*, jclass, jint keyCode)
{
    auto inputSystemAndroid = static_cast<ouzel::input::android::InputSystem*>(ouzel::engine->getInputManager()->getInputSystem());
    auto keyboardDevice = inputSystemAndroid->getKeyboardDevice();
    std::future<bool> f = keyboardDevice->handleKeyPress(convertKeyCode(keyCode));

    if (keyCode == AKEYCODE_BACK)
        return f.get();
    else
        return true;
}

extern "C" JNIEXPORT jboolean JNICALL Java_org_ouzel_OuzelLibJNIWrapper_onKeyUp(JNIEnv*, jclass, jint keyCode)
{
    auto inputSystemAndroid = static_cast<ouzel::input::android::InputSystem*>(ouzel::engine->getInputManager()->getInputSystem());
    auto keyboardDevice = inputSystemAndroid->getKeyboardDevice();
    std::future<bool> f = keyboardDevice->handleKeyRelease(convertKeyCode(keyCode));

    if (keyCode == AKEYCODE_BACK)
        return f.get();
    else
        return true;
}

extern "C" JNIEXPORT jboolean JNICALL Java_org_ouzel_OuzelLibJNIWrapper_onTouchEvent(JNIEnv*, jclass, jobject event)
{
    auto inputSystemAndroid = static_cast<ouzel::input::android::InputSystem*>(ouzel::engine->getInputManager()->getInputSystem());
    return inputSystemAndroid->handleTouchEvent(event);
}

extern "C" JNIEXPORT jboolean JNICALL Java_org_ouzel_OuzelLibJNIWrapper_onGenericMotionEvent(JNIEnv*, jclass, jobject event)
{
    auto inputSystemAndroid = static_cast<ouzel::input::android::InputSystem*>(ouzel::engine->getInputManager()->getInputSystem());
    return inputSystemAndroid->handleGenericMotionEvent(event);
}

namespace ouzel::core::android
{
}
