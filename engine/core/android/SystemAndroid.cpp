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
    fluxion::core::android::System system;
    std::unique_ptr<fluxion::core::android::Engine> engine;

    const std::unordered_map<jint, fluxion::input::Keyboard::Key> keyMap = {
        {AKEYCODE_0, fluxion::input::Keyboard::Key::num0},
        {AKEYCODE_1, fluxion::input::Keyboard::Key::num1},
        {AKEYCODE_2, fluxion::input::Keyboard::Key::num2},
        {AKEYCODE_3, fluxion::input::Keyboard::Key::num3},
        {AKEYCODE_4, fluxion::input::Keyboard::Key::num4},
        {AKEYCODE_5, fluxion::input::Keyboard::Key::num5},
        {AKEYCODE_6, fluxion::input::Keyboard::Key::num6},
        {AKEYCODE_7, fluxion::input::Keyboard::Key::num7},
        {AKEYCODE_8, fluxion::input::Keyboard::Key::num8},
        {AKEYCODE_9, fluxion::input::Keyboard::Key::num9},

        {AKEYCODE_DPAD_UP, fluxion::input::Keyboard::Key::up},
        {AKEYCODE_DPAD_DOWN, fluxion::input::Keyboard::Key::down},
        {AKEYCODE_DPAD_LEFT, fluxion::input::Keyboard::Key::left},
        {AKEYCODE_DPAD_RIGHT, fluxion::input::Keyboard::Key::right},
        {AKEYCODE_DPAD_CENTER, fluxion::input::Keyboard::Key::select},

        {AKEYCODE_A, fluxion::input::Keyboard::Key::a},
        {AKEYCODE_B, fluxion::input::Keyboard::Key::b},
        {AKEYCODE_C, fluxion::input::Keyboard::Key::c},
        {AKEYCODE_D, fluxion::input::Keyboard::Key::d},
        {AKEYCODE_E, fluxion::input::Keyboard::Key::e},
        {AKEYCODE_F, fluxion::input::Keyboard::Key::f},
        {AKEYCODE_G, fluxion::input::Keyboard::Key::g},
        {AKEYCODE_H, fluxion::input::Keyboard::Key::h},
        {AKEYCODE_I, fluxion::input::Keyboard::Key::i},
        {AKEYCODE_J, fluxion::input::Keyboard::Key::j},
        {AKEYCODE_K, fluxion::input::Keyboard::Key::k},
        {AKEYCODE_L, fluxion::input::Keyboard::Key::l},
        {AKEYCODE_M, fluxion::input::Keyboard::Key::m},
        {AKEYCODE_N, fluxion::input::Keyboard::Key::n},
        {AKEYCODE_O, fluxion::input::Keyboard::Key::o},
        {AKEYCODE_P, fluxion::input::Keyboard::Key::p},
        {AKEYCODE_Q, fluxion::input::Keyboard::Key::q},
        {AKEYCODE_R, fluxion::input::Keyboard::Key::r},
        {AKEYCODE_S, fluxion::input::Keyboard::Key::s},
        {AKEYCODE_T, fluxion::input::Keyboard::Key::t},
        {AKEYCODE_U, fluxion::input::Keyboard::Key::u},
        {AKEYCODE_V, fluxion::input::Keyboard::Key::v},
        {AKEYCODE_W, fluxion::input::Keyboard::Key::w},
        {AKEYCODE_X, fluxion::input::Keyboard::Key::x},
        {AKEYCODE_Y, fluxion::input::Keyboard::Key::y},
        {AKEYCODE_Z, fluxion::input::Keyboard::Key::z},

        {AKEYCODE_SEMICOLON, fluxion::input::Keyboard::Key::semicolon},
        {AKEYCODE_EQUALS, fluxion::input::Keyboard::Key::equal},
        {AKEYCODE_COMMA, fluxion::input::Keyboard::Key::comma},
        {AKEYCODE_PERIOD, fluxion::input::Keyboard::Key::period},
        {AKEYCODE_SLASH, fluxion::input::Keyboard::Key::slash},
        {AKEYCODE_GRAVE, fluxion::input::Keyboard::Key::grave},
        {AKEYCODE_LEFT_BRACKET, fluxion::input::Keyboard::Key::leftBracket},
        {AKEYCODE_BACKSLASH, fluxion::input::Keyboard::Key::backslash},
        {AKEYCODE_RIGHT_BRACKET, fluxion::input::Keyboard::Key::rightBracket},
        {AKEYCODE_APOSTROPHE, fluxion::input::Keyboard::Key::quote},
        {AKEYCODE_SPACE, fluxion::input::Keyboard::Key::space},

        {AKEYCODE_DEL, fluxion::input::Keyboard::Key::backspace},
        {AKEYCODE_TAB, fluxion::input::Keyboard::Key::tab},
        {AKEYCODE_ENTER, fluxion::input::Keyboard::Key::enter},
        {AKEYCODE_BREAK, fluxion::input::Keyboard::Key::pause},

        {AKEYCODE_PAGE_UP, fluxion::input::Keyboard::Key::pageUp},
        {AKEYCODE_PAGE_DOWN, fluxion::input::Keyboard::Key::pageDown},

        {AKEYCODE_FORWARD_DEL, fluxion::input::Keyboard::Key::del},

        {AKEYCODE_STAR, fluxion::input::Keyboard::Key::none},
        {AKEYCODE_PLUS, fluxion::input::Keyboard::Key::none},
        {AKEYCODE_MINUS, fluxion::input::Keyboard::Key::minus},

        {AKEYCODE_F1, fluxion::input::Keyboard::Key::f1},
        {AKEYCODE_F2, fluxion::input::Keyboard::Key::f2},
        {AKEYCODE_F3, fluxion::input::Keyboard::Key::f3},
        {AKEYCODE_F4, fluxion::input::Keyboard::Key::f4},
        {AKEYCODE_F5, fluxion::input::Keyboard::Key::f5},
        {AKEYCODE_F6, fluxion::input::Keyboard::Key::f6},
        {AKEYCODE_F7, fluxion::input::Keyboard::Key::f7},
        {AKEYCODE_F8, fluxion::input::Keyboard::Key::f8},
        {AKEYCODE_F9, fluxion::input::Keyboard::Key::f9},
        {AKEYCODE_F10, fluxion::input::Keyboard::Key::f10},
        {AKEYCODE_F11, fluxion::input::Keyboard::Key::f11},
        {AKEYCODE_F12, fluxion::input::Keyboard::Key::f12},

        {AKEYCODE_NUMPAD_0, fluxion::input::Keyboard::Key::numpad0},
        {AKEYCODE_NUMPAD_1, fluxion::input::Keyboard::Key::numpad1},
        {AKEYCODE_NUMPAD_2, fluxion::input::Keyboard::Key::numpad2},
        {AKEYCODE_NUMPAD_3, fluxion::input::Keyboard::Key::numpad3},
        {AKEYCODE_NUMPAD_4, fluxion::input::Keyboard::Key::numpad4},
        {AKEYCODE_NUMPAD_5, fluxion::input::Keyboard::Key::numpad5},
        {AKEYCODE_NUMPAD_6, fluxion::input::Keyboard::Key::numpad6},
        {AKEYCODE_NUMPAD_7, fluxion::input::Keyboard::Key::numpad7},
        {AKEYCODE_NUMPAD_8, fluxion::input::Keyboard::Key::numpad8},
        {AKEYCODE_NUMPAD_9, fluxion::input::Keyboard::Key::numpad9},

        {AKEYCODE_NUMPAD_DIVIDE, fluxion::input::Keyboard::Key::numpadDivide},
        {AKEYCODE_NUMPAD_MULTIPLY, fluxion::input::Keyboard::Key::numpadMultiply},
        {AKEYCODE_NUMPAD_SUBTRACT, fluxion::input::Keyboard::Key::numpadMinus},
        {AKEYCODE_NUMPAD_ADD, fluxion::input::Keyboard::Key::numpadPlus},
        {AKEYCODE_NUMPAD_DOT, fluxion::input::Keyboard::Key::numpadDecimal},
        {AKEYCODE_NUMPAD_COMMA, fluxion::input::Keyboard::Key::numpadSeparator},
        {AKEYCODE_NUMPAD_ENTER, fluxion::input::Keyboard::Key::numpadEnter},
        {AKEYCODE_NUMPAD_EQUALS, fluxion::input::Keyboard::Key::numpadEqual},
        {AKEYCODE_NUMPAD_LEFT_PAREN, fluxion::input::Keyboard::Key::numpadLeftParen},
        {AKEYCODE_NUMPAD_RIGHT_PAREN, fluxion::input::Keyboard::Key::numpadRightParen},

        {AKEYCODE_SHIFT_LEFT, fluxion::input::Keyboard::Key::leftShift},
        {AKEYCODE_SHIFT_RIGHT, fluxion::input::Keyboard::Key::rightShift},
        {AKEYCODE_CTRL_LEFT, fluxion::input::Keyboard::Key::leftControl},
        {AKEYCODE_CTRL_RIGHT, fluxion::input::Keyboard::Key::rightControl},
        {AKEYCODE_ALT_LEFT, fluxion::input::Keyboard::Key::leftAlt},
        {AKEYCODE_ALT_RIGHT, fluxion::input::Keyboard::Key::rightAlt},
        {AKEYCODE_META_LEFT, fluxion::input::Keyboard::Key::leftSuper},
        {AKEYCODE_META_RIGHT, fluxion::input::Keyboard::Key::rightSuper},
        {AKEYCODE_MENU, fluxion::input::Keyboard::Key::menu},
        {AKEYCODE_CLEAR, fluxion::input::Keyboard::Key::clear},
        {AKEYCODE_ESCAPE, fluxion::input::Keyboard::Key::escape},
        {AKEYCODE_CAPS_LOCK, fluxion::input::Keyboard::Key::capsLock},
        {AKEYCODE_SCROLL_LOCK, fluxion::input::Keyboard::Key::scrollLock},
        {AKEYCODE_SYSRQ, fluxion::input::Keyboard::Key::printScreen},
        {AKEYCODE_MOVE_HOME, fluxion::input::Keyboard::Key::home},
        {AKEYCODE_MOVE_END, fluxion::input::Keyboard::Key::end},
        {AKEYCODE_INSERT, fluxion::input::Keyboard::Key::insert},
        {AKEYCODE_SLEEP, fluxion::input::Keyboard::Key::sleep},
        {AKEYCODE_POWER, fluxion::input::Keyboard::Key::power},
        {AKEYCODE_HELP, fluxion::input::Keyboard::Key::help},
        {AKEYCODE_YEN, fluxion::input::Keyboard::Key::yen},
        {AKEYCODE_RO, fluxion::input::Keyboard::Key::ro},
        {AKEYCODE_KANA, fluxion::input::Keyboard::Key::kana},
        {AKEYCODE_KATAKANA_HIRAGANA, fluxion::input::Keyboard::Key::katakanaHiragana},
        {AKEYCODE_MUHENKAN, fluxion::input::Keyboard::Key::muhenkan},
        {AKEYCODE_HENKAN, fluxion::input::Keyboard::Key::henkan},

        {AKEYCODE_HOME, fluxion::input::Keyboard::Key::home},
        {AKEYCODE_BACK, fluxion::input::Keyboard::Key::back},
        {AKEYCODE_FORWARD, fluxion::input::Keyboard::Key::forward},
        {AKEYCODE_SEARCH, fluxion::input::Keyboard::Key::search},
        {AKEYCODE_BOOKMARK, fluxion::input::Keyboard::Key::bookmarks},
        {AKEYCODE_VOLUME_MUTE, fluxion::input::Keyboard::Key::mute},
        {AKEYCODE_VOLUME_UP, fluxion::input::Keyboard::Key::volumeUp},
        {AKEYCODE_VOLUME_DOWN, fluxion::input::Keyboard::Key::volumeDown},
        {AKEYCODE_MEDIA_PLAY, fluxion::input::Keyboard::Key::audioPlay},
        {AKEYCODE_MEDIA_PLAY_PAUSE, fluxion::input::Keyboard::Key::audioPlay},
        {AKEYCODE_MEDIA_STOP, fluxion::input::Keyboard::Key::audioStop},
        {AKEYCODE_MEDIA_NEXT, fluxion::input::Keyboard::Key::audioNext},
        {AKEYCODE_MEDIA_PREVIOUS, fluxion::input::Keyboard::Key::audioPrevious},
        {AKEYCODE_ENVELOPE, fluxion::input::Keyboard::Key::mail}
    };

    fluxion::input::Keyboard::Key convertKeyCode(jint keyCode)
    {
        auto i = keyMap.find(keyCode);

        if (i != keyMap.end())
            return i->second;
        else
            return fluxion::input::Keyboard::Key::none;
    }
}

extern "C" JNIEXPORT jint JNIEXPORT JNI_OnLoad(JavaVM* javaVm, void*)
{
    engine = std::make_unique<fluxion::core::android::Engine>(javaVm);
    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNIEXPORT JNI_OnUnload(JavaVM*, void*)
{
    engine->exit();
    engine.reset();
}

extern "C" JNIEXPORT void JNICALL Java_com_fluxion_FluxionLibJNIWrapper_onCreate(JNIEnv*, jclass, jobject mainActivity)
{
    engine->onCreate(mainActivity);
}

extern "C" JNIEXPORT void JNICALL Java_com_fluxion_FluxionLibJNIWrapper_onSurfaceCreated(JNIEnv*, jclass, jobject surface)
{
    try
    {
        engine->onSurfaceCreated(surface);

        if (!engine->isActive()) engine->run();
    }
    catch (const std::exception& e)
    {
        fluxion::log(fluxion::Log::Level::error) << e.what();
    }
}

extern "C" JNIEXPORT void JNICALL Java_com_fluxion_FluxionLibJNIWrapper_onSurfaceDestroyed(JNIEnv*, jclass)
{
    engine->onSurfaceDestroyed();
}

extern "C" JNIEXPORT void JNICALL Java_com_fluxion_FluxionLibJNIWrapper_onSurfaceChanged(JNIEnv*, jclass, jobject, jint width, jint height)
{
    const auto& window = engine->getWindow().getNativeWindow();
    window.handleResize(fluxion::math::Size<std::uint32_t, 2>{static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height)});
}

extern "C" JNIEXPORT void JNICALL Java_com_fluxion_FluxionLibJNIWrapper_onStart(JNIEnv*, jclass)
{
    // Do nothing
}

extern "C" JNIEXPORT void JNICALL Java_com_fluxion_FluxionLibJNIWrapper_onPause(JNIEnv*, jclass)
{
    engine->pause();
}

extern "C" JNIEXPORT void JNICALL Java_com_fluxion_FluxionLibJNIWrapper_onResume(JNIEnv*, jclass)
{
    engine->resume();
}

extern "C" JNIEXPORT void JNICALL Java_com_fluxion_FluxionLibJNIWrapper_onConfigurationChanged(JNIEnv*, jclass, jobject newConfig)
{
    engine->onConfigurationChanged(newConfig);
}

extern "C" JNIEXPORT void JNICALL Java_com_fluxion_FluxionLibJNIWrapper_onLowMemory(JNIEnv*, jclass)
{
    auto event = std::make_unique<fluxion::SystemEvent>();
    event->type = fluxion::Event::Type::lowMemory;
    engine->getEventDispatcher().postEvent(std::move(event));
}

extern "C" JNIEXPORT jboolean JNICALL Java_com_fluxion_FluxionLibJNIWrapper_onKeyDown(JNIEnv*, jclass, jint keyCode)
{
    auto& inputSystem = fluxion::engine->getInputManager().getInputSystem();
    const auto keyboardDevice = inputSystem.getKeyboardDevice();
    auto f = keyboardDevice->handleKeyPress(convertKeyCode(keyCode));

    if (keyCode == AKEYCODE_BACK)
        return f.get();
    else
        return true;
}

extern "C" JNIEXPORT jboolean JNICALL Java_com_fluxion_FluxionLibJNIWrapper_onKeyUp(JNIEnv*, jclass, jint keyCode)
{
    auto& inputSystem = fluxion::engine->getInputManager().getInputSystem();
    const auto keyboardDevice = inputSystem.getKeyboardDevice();
    auto f = keyboardDevice->handleKeyRelease(convertKeyCode(keyCode));

    if (keyCode == AKEYCODE_BACK)
        return f.get();
    else
        return true;
}

extern "C" JNIEXPORT jboolean JNICALL Java_com_fluxion_FluxionLibJNIWrapper_onTouchEvent(JNIEnv*, jclass, jobject event)
{
    auto& inputSystem = fluxion::engine->getInputManager().getInputSystem();
    return inputSystem.handleTouchEvent(event);
}

extern "C" JNIEXPORT jboolean JNICALL Java_com_fluxion_FluxionLibJNIWrapper_onGenericMotionEvent(JNIEnv*, jclass, jobject event)
{
    auto& inputSystem = fluxion::engine->getInputManager().getInputSystem();
    return inputSystem.handleGenericMotionEvent(event);
}

namespace fluxion::core::android
{
    System::System():
        core::System{std::vector<std::string>{}}
    {
    }
}
