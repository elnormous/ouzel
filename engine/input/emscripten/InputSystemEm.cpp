// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include <map>
#include <emscripten.h>
#include "InputSystemEm.hpp"
#include "GamepadDeviceEm.hpp"
#include "core/Engine.hpp"
#include "utils/Log.hpp"

namespace
{
    const std::map<std::string, ouzel::input::Keyboard::Key> keyMap = {
        {"Backspace", ouzel::input::Keyboard::Key::Backspace},
        {"Tab", ouzel::input::Keyboard::Key::Tab},
        {"Enter", ouzel::input::Keyboard::Key::Enter},
        {"ShiftLeft", ouzel::input::Keyboard::Key::LeftShift},
        {"ShiftRight", ouzel::input::Keyboard::Key::RightShift},
        {"ControlLeft", ouzel::input::Keyboard::Key::LeftControl},
        {"ControlRight", ouzel::input::Keyboard::Key::RightControl},
        {"AltLeft", ouzel::input::Keyboard::Key::LeftAlt},
        {"AltRight", ouzel::input::Keyboard::Key::RightAlt},
        {"MetaLeft", ouzel::input::Keyboard::Key::LeftSuper},
        {"MetaRight", ouzel::input::Keyboard::Key::RightSuper},
        {"Pause", ouzel::input::Keyboard::Key::Pause},
        {"CapsLock", ouzel::input::Keyboard::Key::CapsLock},
        {"Escape", ouzel::input::Keyboard::Key::Escape},
        {"PageUp", ouzel::input::Keyboard::Key::PageUp},
        {"PageDown", ouzel::input::Keyboard::Key::PageDown},
        {"ContextMenu", ouzel::input::Keyboard::Key::Menu},
        {"Help", ouzel::input::Keyboard::Key::Help},
        {"Home", ouzel::input::Keyboard::Key::Home},
        {"End", ouzel::input::Keyboard::Key::End},
        {"ArrowLeft", ouzel::input::Keyboard::Key::Left},
        {"ArrowUp", ouzel::input::Keyboard::Key::Up},
        {"ArrowRight", ouzel::input::Keyboard::Key::Right},
        {"ArrowDown", ouzel::input::Keyboard::Key::Down},
        {"Insert", ouzel::input::Keyboard::Key::Insert},
        {"Delete", ouzel::input::Keyboard::Key::Del},
        {"Digit0", ouzel::input::Keyboard::Key::Num0},
        {"Digit1", ouzel::input::Keyboard::Key::Num1},
        {"Digit2", ouzel::input::Keyboard::Key::Num2},
        {"Digit3", ouzel::input::Keyboard::Key::Num3},
        {"Digit4", ouzel::input::Keyboard::Key::Num4},
        {"Digit5", ouzel::input::Keyboard::Key::Num5},
        {"Digit6", ouzel::input::Keyboard::Key::Num6},
        {"Digit7", ouzel::input::Keyboard::Key::Num7},
        {"Digit8", ouzel::input::Keyboard::Key::Num8},
        {"Digit9", ouzel::input::Keyboard::Key::Num9},
        {"KeyA", ouzel::input::Keyboard::Key::A},
        {"KeyB", ouzel::input::Keyboard::Key::B},
        {"KeyC", ouzel::input::Keyboard::Key::C},
        {"KeyD", ouzel::input::Keyboard::Key::D},
        {"KeyE", ouzel::input::Keyboard::Key::E},
        {"KeyF", ouzel::input::Keyboard::Key::F},
        {"KeyG", ouzel::input::Keyboard::Key::G},
        {"KeyH", ouzel::input::Keyboard::Key::H},
        {"KeyI", ouzel::input::Keyboard::Key::I},
        {"KeyJ", ouzel::input::Keyboard::Key::J},
        {"KeyK", ouzel::input::Keyboard::Key::K},
        {"KeyL", ouzel::input::Keyboard::Key::L},
        {"KeyM", ouzel::input::Keyboard::Key::M},
        {"KeyN", ouzel::input::Keyboard::Key::N},
        {"KeyO", ouzel::input::Keyboard::Key::O},
        {"KeyP", ouzel::input::Keyboard::Key::P},
        {"KeyQ", ouzel::input::Keyboard::Key::Q},
        {"KeyR", ouzel::input::Keyboard::Key::R},
        {"KeyS", ouzel::input::Keyboard::Key::S},
        {"KeyT", ouzel::input::Keyboard::Key::T},
        {"KeyU", ouzel::input::Keyboard::Key::U},
        {"KeyV", ouzel::input::Keyboard::Key::V},
        {"KeyW", ouzel::input::Keyboard::Key::W},
        {"KeyX", ouzel::input::Keyboard::Key::X},
        {"KeyY", ouzel::input::Keyboard::Key::Y},
        {"KeyZ", ouzel::input::Keyboard::Key::Z},
        {"OSLeft", ouzel::input::Keyboard::Key::LeftSuper},
        {"OSRight", ouzel::input::Keyboard::Key::RightSuper},
        {"Delete", ouzel::input::Keyboard::Key::Del},
        {"NumLock", ouzel::input::Keyboard::Key::NumLock},
        {"NumpadEnter", ouzel::input::Keyboard::Key::NumpadEnter},
        {"Numpad0", ouzel::input::Keyboard::Key::Numpad0},
        {"Numpad1", ouzel::input::Keyboard::Key::Numpad1},
        {"Numpad2", ouzel::input::Keyboard::Key::Numpad2},
        {"Numpad3", ouzel::input::Keyboard::Key::Numpad3},
        {"Numpad4", ouzel::input::Keyboard::Key::Numpad4},
        {"Numpad5", ouzel::input::Keyboard::Key::Numpad5},
        {"Numpad6", ouzel::input::Keyboard::Key::Numpad6},
        {"Numpad7", ouzel::input::Keyboard::Key::Numpad7},
        {"Numpad8", ouzel::input::Keyboard::Key::Numpad8},
        {"Numpad9", ouzel::input::Keyboard::Key::Numpad9},
        {"NumpadMultiply", ouzel::input::Keyboard::Key::NumpadMultiply},
        {"NumpadAdd", ouzel::input::Keyboard::Key::NumpadPlus},
        {"NumpadSubtract", ouzel::input::Keyboard::Key::NumpadMinus},
        {"NumpadComma", ouzel::input::Keyboard::Key::NumpadSeparator},
        {"NumpadDecimal", ouzel::input::Keyboard::Key::NumpadDecimal},
        {"NumpadDivide", ouzel::input::Keyboard::Key::NumpadDivide},
        {"NumpadEqual", ouzel::input::Keyboard::Key::NumpadEqual},
        {"F1", ouzel::input::Keyboard::Key::F1},
        {"F2", ouzel::input::Keyboard::Key::F2},
        {"F3", ouzel::input::Keyboard::Key::F3},
        {"F4", ouzel::input::Keyboard::Key::F4},
        {"F5", ouzel::input::Keyboard::Key::F5},
        {"F6", ouzel::input::Keyboard::Key::F6},
        {"F7", ouzel::input::Keyboard::Key::F7},
        {"F8", ouzel::input::Keyboard::Key::F8},
        {"F9", ouzel::input::Keyboard::Key::F9},
        {"F10", ouzel::input::Keyboard::Key::F10},
        {"F11", ouzel::input::Keyboard::Key::F11},
        {"F12", ouzel::input::Keyboard::Key::F12},
        {"F13", ouzel::input::Keyboard::Key::F13},
        {"F14", ouzel::input::Keyboard::Key::F14},
        {"F15", ouzel::input::Keyboard::Key::F15},
        {"F16", ouzel::input::Keyboard::Key::F16},
        {"F17", ouzel::input::Keyboard::Key::F17},
        {"F18", ouzel::input::Keyboard::Key::F18},
        {"F19", ouzel::input::Keyboard::Key::F19},
        {"F20", ouzel::input::Keyboard::Key::F20},
        {"F21", ouzel::input::Keyboard::Key::F21},
        {"F22", ouzel::input::Keyboard::Key::F22},
        {"F23", ouzel::input::Keyboard::Key::F23},
        {"F24", ouzel::input::Keyboard::Key::F24},
        {"ScrollLock", ouzel::input::Keyboard::Key::ScrollLock},
        {"Semicolon", ouzel::input::Keyboard::Key::Semicolon},
        {"Equal", ouzel::input::Keyboard::Key::Equal},
        {"Comma", ouzel::input::Keyboard::Key::Comma},
        {"Minus", ouzel::input::Keyboard::Key::Minus},
        {"Period", ouzel::input::Keyboard::Key::Period},
        {"Slash", ouzel::input::Keyboard::Key::Slash},
        {"Backquote", ouzel::input::Keyboard::Key::Grave},
        {"BracketLeft", ouzel::input::Keyboard::Key::LeftBracket},
        {"Backslash", ouzel::input::Keyboard::Key::Backslash},
        {"BracketRight", ouzel::input::Keyboard::Key::RightBracket},
        {"Quote", ouzel::input::Keyboard::Key::Quote},
        {"IntlBackslash", ouzel::input::Keyboard::Key::IntlBackslash},
        {"PrintScreen", ouzel::input::Keyboard::Key::PrintScreen},
        {"IntlRo", ouzel::input::Keyboard::Key::Ro},
        {"IntlYen", ouzel::input::Keyboard::Key::Yen}
    };

    ouzel::input::Keyboard::Key convertKeyCode(const EM_UTF8 key[32])
    {
        auto i = keyMap.find(key);

        if (i != keyMap.end())
            return i->second;
        else
            return ouzel::input::Keyboard::Key::Unknown;
    }

    EM_BOOL emKeyCallback(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData)
    {
        ouzel::input::KeyboardDevice* keyboardDevice = static_cast<ouzel::input::KeyboardDevice*>(userData);

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
        ouzel::input::MouseDevice* mouseDevice = static_cast<ouzel::input::MouseDevice*>(userData);
        ouzel::input::Mouse::Button button;

        switch (mouseEvent->button)
        {
            case 0:
                button = ouzel::input::Mouse::Button::Left;
                break;
            case 1:
                button = ouzel::input::Mouse::Button::Middle;
                break;
            case 2:
                button = ouzel::input::Mouse::Button::Right;
                break;
            default:
                button = ouzel::input::Mouse::Button::Unknown;
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
        ouzel::input::MouseDevice* mouseDevice = static_cast<ouzel::input::MouseDevice*>(userData);

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
        ouzel::input::MouseDevice* mouseDevice = static_cast<ouzel::input::MouseDevice*>(userData);

        if (eventType == EMSCRIPTEN_EVENT_POINTERLOCKCHANGE)
        {
            mouseDevice->handleCursorLockChange(pointerlockChangeEvent->isActive);
            return true;
        }

        return false;
    }

    EM_BOOL emGamepadCallback(int eventType, const EmscriptenGamepadEvent* gamepadEvent, void* userData)
    {
        ouzel::input::InputSystemEm* inputEm = static_cast<ouzel::input::InputSystemEm*>(userData);

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
        ouzel::input::TouchpadDevice* touchpadDevice = static_cast<ouzel::input::TouchpadDevice*>(userData);

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

namespace ouzel
{
    namespace input
    {
        InputSystemEm::InputSystemEm(const std::function<std::future<bool>(const Event&)>& initCallback):
            InputSystem(initCallback),
            keyboardDevice(std::make_unique<KeyboardDevice>(*this, getNextDeviceId())),
            mouseDevice(std::make_unique<MouseDeviceEm>(*this, getNextDeviceId())),
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
                engine->log(Log::Level::Info) << "Gamepads not supported";
            else
            {
                for (long index = 0; index < result; ++index)
                    handleGamepadConnected(index);
            }
        }

        void InputSystemEm::executeCommand(const Command& command)
        {
            switch (command.type)
            {
                case Command::Type::SetPlayerIndex:
                {
                    break;
                }
                case Command::Type::SetVibration:
                {
                    break;
                }
                case Command::Type::SetPosition:
                {
                    break;
                }
                case Command::Type::SetCursor:
                {
                    break;
                }
                case Command::Type::SetCursorVisible:
                {
                    if (InputDevice* inputDevice = getInputDevice(command.deviceId))
                    {
                        if (inputDevice == mouseDevice.get())
                            mouseDevice->setCursorVisible(command.locked);
                    }
                    break;
                }
                case Command::Type::SetCursorLocked:
                {
                    if (InputDevice* inputDevice = getInputDevice(command.deviceId))
                    {
                        if (inputDevice == mouseDevice.get())
                            mouseDevice->setCursorLocked(command.locked);
                    }
                    break;
                }
                case Command::Type::ShowVirtualKeyboard:
                    break;
                case Command::Type::HideVirtualKeyboard:
                    break;
                default:
                    break;
            }
        }

        void InputSystemEm::update()
        {
            for (const auto& i : gamepadDevices)
            {
                GamepadDeviceEm* gamepadDevice = static_cast<GamepadDeviceEm*>(i.second.get());
                gamepadDevice->update();
            }
        }

        void InputSystemEm::handleGamepadConnected(long index)
        {
            auto gamepadDevice = std::make_unique<GamepadDeviceEm>(*this, getNextDeviceId(), index);
            gamepadDevices.insert(std::make_pair(index, std::move(gamepadDevice)));
        }

        void InputSystemEm::handleGamepadDisconnected(long index)
        {
            auto i = gamepadDevices.find(index);

            if (i != gamepadDevices.end())
                gamepadDevices.erase(i);
        }
    } // namespace input
} // namespace ouzel
