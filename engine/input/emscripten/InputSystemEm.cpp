// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include <map>
#include <emscripten.h>
#include "InputSystemEm.hpp"
#include "GamepadDeviceEm.hpp"
#include "../../core/Engine.hpp"
#include "../../utils/Log.hpp"

namespace
{
    const std::map<std::string, ouzel::input::Keyboard::Key> keyMap = {
        {"Backspace", ouzel::input::Keyboard::Key::backspace},
        {"Tab", ouzel::input::Keyboard::Key::tab},
        {"Enter", ouzel::input::Keyboard::Key::enter},
        {"ShiftLeft", ouzel::input::Keyboard::Key::leftShift},
        {"ShiftRight", ouzel::input::Keyboard::Key::rightShift},
        {"ControlLeft", ouzel::input::Keyboard::Key::leftControl},
        {"ControlRight", ouzel::input::Keyboard::Key::rightControl},
        {"AltLeft", ouzel::input::Keyboard::Key::leftAlt},
        {"AltRight", ouzel::input::Keyboard::Key::rightAlt},
        {"MetaLeft", ouzel::input::Keyboard::Key::leftSuper},
        {"MetaRight", ouzel::input::Keyboard::Key::rightSuper},
        {"Pause", ouzel::input::Keyboard::Key::pause},
        {"CapsLock", ouzel::input::Keyboard::Key::capsLock},
        {"Escape", ouzel::input::Keyboard::Key::escape},
        {"PageUp", ouzel::input::Keyboard::Key::pageUp},
        {"PageDown", ouzel::input::Keyboard::Key::pageDown},
        {"ContextMenu", ouzel::input::Keyboard::Key::menu},
        {"Help", ouzel::input::Keyboard::Key::help},
        {"Home", ouzel::input::Keyboard::Key::home},
        {"End", ouzel::input::Keyboard::Key::end},
        {"ArrowLeft", ouzel::input::Keyboard::Key::left},
        {"ArrowUp", ouzel::input::Keyboard::Key::up},
        {"ArrowRight", ouzel::input::Keyboard::Key::right},
        {"ArrowDown", ouzel::input::Keyboard::Key::down},
        {"Insert", ouzel::input::Keyboard::Key::insert},
        {"Delete", ouzel::input::Keyboard::Key::del},
        {"Digit0", ouzel::input::Keyboard::Key::num0},
        {"Digit1", ouzel::input::Keyboard::Key::num1},
        {"Digit2", ouzel::input::Keyboard::Key::num2},
        {"Digit3", ouzel::input::Keyboard::Key::num3},
        {"Digit4", ouzel::input::Keyboard::Key::num4},
        {"Digit5", ouzel::input::Keyboard::Key::num5},
        {"Digit6", ouzel::input::Keyboard::Key::num6},
        {"Digit7", ouzel::input::Keyboard::Key::num7},
        {"Digit8", ouzel::input::Keyboard::Key::num8},
        {"Digit9", ouzel::input::Keyboard::Key::num9},
        {"KeyA", ouzel::input::Keyboard::Key::a},
        {"KeyB", ouzel::input::Keyboard::Key::b},
        {"KeyC", ouzel::input::Keyboard::Key::c},
        {"KeyD", ouzel::input::Keyboard::Key::d},
        {"KeyE", ouzel::input::Keyboard::Key::e},
        {"KeyF", ouzel::input::Keyboard::Key::f},
        {"KeyG", ouzel::input::Keyboard::Key::g},
        {"KeyH", ouzel::input::Keyboard::Key::h},
        {"KeyI", ouzel::input::Keyboard::Key::i},
        {"KeyJ", ouzel::input::Keyboard::Key::j},
        {"KeyK", ouzel::input::Keyboard::Key::k},
        {"KeyL", ouzel::input::Keyboard::Key::l},
        {"KeyM", ouzel::input::Keyboard::Key::m},
        {"KeyN", ouzel::input::Keyboard::Key::n},
        {"KeyO", ouzel::input::Keyboard::Key::o},
        {"KeyP", ouzel::input::Keyboard::Key::p},
        {"KeyQ", ouzel::input::Keyboard::Key::q},
        {"KeyR", ouzel::input::Keyboard::Key::r},
        {"KeyS", ouzel::input::Keyboard::Key::s},
        {"KeyT", ouzel::input::Keyboard::Key::t},
        {"KeyU", ouzel::input::Keyboard::Key::u},
        {"KeyV", ouzel::input::Keyboard::Key::v},
        {"KeyW", ouzel::input::Keyboard::Key::w},
        {"KeyX", ouzel::input::Keyboard::Key::x},
        {"KeyY", ouzel::input::Keyboard::Key::y},
        {"KeyZ", ouzel::input::Keyboard::Key::z},
        {"OSLeft", ouzel::input::Keyboard::Key::leftSuper},
        {"OSRight", ouzel::input::Keyboard::Key::rightSuper},
        {"Delete", ouzel::input::Keyboard::Key::del},
        {"NumLock", ouzel::input::Keyboard::Key::numLock},
        {"NumpadEnter", ouzel::input::Keyboard::Key::numpadEnter},
        {"Numpad0", ouzel::input::Keyboard::Key::numpad0},
        {"Numpad1", ouzel::input::Keyboard::Key::numpad1},
        {"Numpad2", ouzel::input::Keyboard::Key::numpad2},
        {"Numpad3", ouzel::input::Keyboard::Key::numpad3},
        {"Numpad4", ouzel::input::Keyboard::Key::numpad4},
        {"Numpad5", ouzel::input::Keyboard::Key::numpad5},
        {"Numpad6", ouzel::input::Keyboard::Key::numpad6},
        {"Numpad7", ouzel::input::Keyboard::Key::numpad7},
        {"Numpad8", ouzel::input::Keyboard::Key::numpad8},
        {"Numpad9", ouzel::input::Keyboard::Key::numpad9},
        {"NumpadMultiply", ouzel::input::Keyboard::Key::numpadMultiply},
        {"NumpadAdd", ouzel::input::Keyboard::Key::numpadPlus},
        {"NumpadSubtract", ouzel::input::Keyboard::Key::numpadMinus},
        {"NumpadComma", ouzel::input::Keyboard::Key::numpadSeparator},
        {"NumpadDecimal", ouzel::input::Keyboard::Key::numpadDecimal},
        {"NumpadDivide", ouzel::input::Keyboard::Key::numpadDivide},
        {"NumpadEqual", ouzel::input::Keyboard::Key::numpadEqual},
        {"F1", ouzel::input::Keyboard::Key::f1},
        {"F2", ouzel::input::Keyboard::Key::f2},
        {"F3", ouzel::input::Keyboard::Key::f3},
        {"F4", ouzel::input::Keyboard::Key::f4},
        {"F5", ouzel::input::Keyboard::Key::f5},
        {"F6", ouzel::input::Keyboard::Key::f6},
        {"F7", ouzel::input::Keyboard::Key::f7},
        {"F8", ouzel::input::Keyboard::Key::f8},
        {"F9", ouzel::input::Keyboard::Key::f9},
        {"F10", ouzel::input::Keyboard::Key::f10},
        {"F11", ouzel::input::Keyboard::Key::f11},
        {"F12", ouzel::input::Keyboard::Key::f12},
        {"F13", ouzel::input::Keyboard::Key::f13},
        {"F14", ouzel::input::Keyboard::Key::f14},
        {"F15", ouzel::input::Keyboard::Key::f15},
        {"F16", ouzel::input::Keyboard::Key::f16},
        {"F17", ouzel::input::Keyboard::Key::f17},
        {"F18", ouzel::input::Keyboard::Key::f18},
        {"F19", ouzel::input::Keyboard::Key::f19},
        {"F20", ouzel::input::Keyboard::Key::f20},
        {"F21", ouzel::input::Keyboard::Key::f21},
        {"F22", ouzel::input::Keyboard::Key::f22},
        {"F23", ouzel::input::Keyboard::Key::f23},
        {"F24", ouzel::input::Keyboard::Key::f24},
        {"ScrollLock", ouzel::input::Keyboard::Key::scrollLock},
        {"Semicolon", ouzel::input::Keyboard::Key::semicolon},
        {"Equal", ouzel::input::Keyboard::Key::equal},
        {"Comma", ouzel::input::Keyboard::Key::comma},
        {"Minus", ouzel::input::Keyboard::Key::minus},
        {"Period", ouzel::input::Keyboard::Key::period},
        {"Slash", ouzel::input::Keyboard::Key::slash},
        {"Backquote", ouzel::input::Keyboard::Key::grave},
        {"BracketLeft", ouzel::input::Keyboard::Key::leftBracket},
        {"Backslash", ouzel::input::Keyboard::Key::backslash},
        {"BracketRight", ouzel::input::Keyboard::Key::rightBracket},
        {"Quote", ouzel::input::Keyboard::Key::quote},
        {"IntlBackslash", ouzel::input::Keyboard::Key::intlBackslash},
        {"PrintScreen", ouzel::input::Keyboard::Key::printScreen},
        {"IntlRo", ouzel::input::Keyboard::Key::ro},
        {"IntlYen", ouzel::input::Keyboard::Key::yen}
    };

    ouzel::input::Keyboard::Key convertKeyCode(const EM_UTF8 key[32])
    {
        auto i = keyMap.find(key);

        if (i != keyMap.end())
            return i->second;
        else
            return ouzel::input::Keyboard::Key::none;
    }

    EM_BOOL emKeyCallback(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData)
    {
        auto keyboardDevice = static_cast<ouzel::input::KeyboardDevice*>(userData);

        switch (eventType)
        {
            case EMSCRIPTEN_EVENT_KEYPRESS:
            case EMSCRIPTEN_EVENT_KEYDOWN:
                keyboardDevice->handleKeyPress(convertKeyCode(keyEvent->code));
                return true;
            case EMSCRIPTEN_EVENT_KEYUP:
                keyboardDevice->handleKeyRelease(convertKeyCode(keyEvent->code));
                return true;
        }

        return false;
    }

    EM_BOOL emMouseCallback(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData)
    {
        auto mouseDevice = static_cast<ouzel::input::MouseDevice*>(userData);
        ouzel::input::Mouse::Button button;

        switch (mouseEvent->button)
        {
            case 0:
                button = ouzel::input::Mouse::Button::left;
                break;
            case 1:
                button = ouzel::input::Mouse::Button::middle;
                break;
            case 2:
                button = ouzel::input::Mouse::Button::right;
                break;
            default:
                button = ouzel::input::Mouse::Button::none;
                break;
        }

        ouzel::Vector2F position(static_cast<float>(mouseEvent->canvasX),
                                 static_cast<float>(mouseEvent->canvasY));

        switch (eventType)
        {
            case EMSCRIPTEN_EVENT_MOUSEDOWN:
                mouseDevice->handleButtonPress(button,
                                               ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position));
                return true;
            case EMSCRIPTEN_EVENT_MOUSEUP:
                mouseDevice->handleButtonRelease(button,
                                                 ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position));
                return true;
            case EMSCRIPTEN_EVENT_MOUSEMOVE:
                mouseDevice->handleMove(ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position));
                return true;
        }

        return false;
    }

    EM_BOOL emWheelCallback(int eventType, const EmscriptenWheelEvent* wheelEvent, void* userData)
    {
        auto mouseDevice = static_cast<ouzel::input::MouseDevice*>(userData);

        if (eventType == EMSCRIPTEN_EVENT_WHEEL)
        {
            ouzel::Vector2F position(static_cast<float>(wheelEvent->mouse.canvasX),
                                    static_cast<float>(wheelEvent->mouse.canvasY));

            mouseDevice->handleScroll(ouzel::Vector2F(static_cast<float>(wheelEvent->deltaX), static_cast<float>(wheelEvent->deltaY)),
                                      ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position));

            return true;
        }

        return false;
    }

    EM_BOOL emPointerLockChangeCallback(int eventType, const EmscriptenPointerlockChangeEvent* pointerlockChangeEvent, void* userData)
    {
        auto mouseDevice = static_cast<ouzel::input::MouseDevice*>(userData);

        if (eventType == EMSCRIPTEN_EVENT_POINTERLOCKCHANGE)
        {
            mouseDevice->handleCursorLockChange(pointerlockChangeEvent->isActive);
            return true;
        }

        return false;
    }

    EM_BOOL emGamepadCallback(int eventType, const EmscriptenGamepadEvent* gamepadEvent, void* userData)
    {
        auto inputEm = static_cast<ouzel::input::emscripten::InputSystem*>(userData);

        if (eventType == EMSCRIPTEN_EVENT_GAMEPADCONNECTED)
        {
            inputEm->handleGamepadConnected(gamepadEvent->index);
            return true;
        }
        else if (eventType == EMSCRIPTEN_EVENT_GAMEPADDISCONNECTED)
        {
            inputEm->handleGamepadDisconnected(gamepadEvent->index);
            return true;
        }

        return false;
    }

    EM_BOOL emTouchCallback(int eventType, const EmscriptenTouchEvent* touchEvent, void* userData)
    {
        auto touchpadDevice = static_cast<ouzel::input::TouchpadDevice*>(userData);

        for (int i = 0; i < touchEvent->numTouches; ++i)
        {
            if (touchEvent->touches[i].isChanged)
            {
                ouzel::Vector2F position(static_cast<float>(touchEvent->touches[i].canvasX),
                                        static_cast<float>(touchEvent->touches[i].canvasY));

                switch (eventType)
                {
                    case EMSCRIPTEN_EVENT_TOUCHSTART:
                        touchpadDevice->handleTouchBegin(static_cast<std::uint64_t>(touchEvent->touches[i].identifier),
                                                         ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position),
                                                         1.0F);
                        break;
                    case EMSCRIPTEN_EVENT_TOUCHEND:
                        touchpadDevice->handleTouchEnd(static_cast<std::uint64_t>(touchEvent->touches[i].identifier),
                                                       ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position),
                                                       1.0F);
                        break;
                    case EMSCRIPTEN_EVENT_TOUCHMOVE:
                        touchpadDevice->handleTouchMove(static_cast<std::uint64_t>(touchEvent->touches[i].identifier),
                                                        ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position),
                                                        1.0F);
                        break;
                    case EMSCRIPTEN_EVENT_TOUCHCANCEL:
                        touchpadDevice->handleTouchCancel(static_cast<std::uint64_t>(touchEvent->touches[i].identifier),
                                                          ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position),
                                                          1.0F);
                        break;
                }
            }
        }

        return false;
    }
}

namespace ouzel::input::emscripten
{
    InputSystem::InputSystem(const std::function<std::future<bool>(const Event&)>& initCallback):
        input::InputSystem(initCallback),
        keyboardDevice(std::make_unique<KeyboardDevice>(*this, getNextDeviceId())),
        mouseDevice(std::make_unique<MouseDevice>(*this, getNextDeviceId())),
        touchpadDevice(std::make_unique<TouchpadDevice>(*this, getNextDeviceId(), true))
    {
        emscripten_set_keypress_callback(nullptr, keyboardDevice.get(), true, emKeyCallback);
        emscripten_set_keydown_callback(nullptr, keyboardDevice.get(), true, emKeyCallback);
        emscripten_set_keyup_callback(nullptr, keyboardDevice.get(), true, emKeyCallback);

        emscripten_set_mousedown_callback("#canvas", mouseDevice.get(), true, emMouseCallback);
        emscripten_set_mouseup_callback("#canvas", mouseDevice.get(), true, emMouseCallback);
        emscripten_set_mousemove_callback("#canvas", mouseDevice.get(), true, emMouseCallback);
        emscripten_set_wheel_callback("#canvas", mouseDevice.get(), true, emWheelCallback);
        emscripten_set_pointerlockchange_callback(nullptr, mouseDevice.get(), true, emPointerLockChangeCallback);

        emscripten_set_gamepadconnected_callback(this, true, emGamepadCallback);
        emscripten_set_gamepaddisconnected_callback(this, true, emGamepadCallback);

        emscripten_set_touchstart_callback("#canvas", touchpadDevice.get(), true, emTouchCallback);
        emscripten_set_touchend_callback("#canvas", touchpadDevice.get(), true, emTouchCallback);
        emscripten_set_touchmove_callback("#canvas", touchpadDevice.get(), true, emTouchCallback);
        emscripten_set_touchcancel_callback("#canvas", touchpadDevice.get(), true, emTouchCallback);

        const int result = emscripten_get_num_gamepads();

        if (result == EMSCRIPTEN_RESULT_NOT_SUPPORTED)
            logger.log(Log::Level::info) << "Gamepads not supported";
        else
        {
            for (long index = 0; index < result; ++index)
                handleGamepadConnected(index);
        }
    }

    void InputSystem::executeCommand(const Command& command)
    {
        switch (command.type)
        {
            case Command::Type::setPlayerIndex:
            {
                break;
            }
            case Command::Type::setVibration:
            {
                break;
            }
            case Command::Type::setPosition:
            {
                break;
            }
            case Command::Type::setCursor:
            {
                break;
            }
            case Command::Type::setCursorVisible:
            {
                if (InputDevice* inputDevice = getInputDevice(command.deviceId))
                {
                    if (inputDevice == mouseDevice.get())
                        mouseDevice->setCursorVisible(command.locked);
                }
                break;
            }
            case Command::Type::setCursorLocked:
            {
                if (InputDevice* inputDevice = getInputDevice(command.deviceId))
                {
                    if (inputDevice == mouseDevice.get())
                        mouseDevice->setCursorLocked(command.locked);
                }
                break;
            }
            case Command::Type::showVirtualKeyboard:
                break;
            case Command::Type::hideVirtualKeyboard:
                break;
            default:
                break;
        }
    }

    void InputSystem::update()
    {
        for (const auto& i : gamepadDevices)
        {
            auto gamepadDevice = static_cast<GamepadDevice*>(i.second.get());
            gamepadDevice->update();
        }
    }

    void InputSystem::handleGamepadConnected(long index)
    {
        auto gamepadDevice = std::make_unique<GamepadDevice>(*this, getNextDeviceId(), index);
        gamepadDevices.insert(std::pair(index, std::move(gamepadDevice)));
    }

    void InputSystem::handleGamepadDisconnected(long index)
    {
        auto i = gamepadDevices.find(index);

        if (i != gamepadDevices.end())
            gamepadDevices.erase(i);
    }
}
