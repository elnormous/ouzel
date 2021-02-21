// Ouzel by Elviss Strazdins

#include <map>
#include <emscripten.h>
#include "InputSystemEm.hpp"
#include "GamepadDeviceEm.hpp"
#include "../../core/Engine.hpp"
#include "../../utils/Log.hpp"

namespace ouzel::input::emscripten
{
    namespace
    {
        const std::map<std::string, Keyboard::Key> keyMap = {
            {"Backspace", Keyboard::Key::backspace},
            {"Tab", Keyboard::Key::tab},
            {"Enter", Keyboard::Key::enter},
            {"ShiftLeft", Keyboard::Key::leftShift},
            {"ShiftRight", Keyboard::Key::rightShift},
            {"ControlLeft", Keyboard::Key::leftControl},
            {"ControlRight", Keyboard::Key::rightControl},
            {"AltLeft", Keyboard::Key::leftAlt},
            {"AltRight", Keyboard::Key::rightAlt},
            {"MetaLeft", Keyboard::Key::leftSuper},
            {"MetaRight", Keyboard::Key::rightSuper},
            {"Pause", Keyboard::Key::pause},
            {"CapsLock", Keyboard::Key::capsLock},
            {"Escape", Keyboard::Key::escape},
            {"PageUp", Keyboard::Key::pageUp},
            {"PageDown", Keyboard::Key::pageDown},
            {"ContextMenu", Keyboard::Key::menu},
            {"Help", Keyboard::Key::help},
            {"Home", Keyboard::Key::home},
            {"End", Keyboard::Key::end},
            {"ArrowLeft", Keyboard::Key::left},
            {"ArrowUp", Keyboard::Key::up},
            {"ArrowRight", Keyboard::Key::right},
            {"ArrowDown", Keyboard::Key::down},
            {"Insert", Keyboard::Key::insert},
            {"Delete", Keyboard::Key::del},
            {"Digit0", Keyboard::Key::num0},
            {"Digit1", Keyboard::Key::num1},
            {"Digit2", Keyboard::Key::num2},
            {"Digit3", Keyboard::Key::num3},
            {"Digit4", Keyboard::Key::num4},
            {"Digit5", Keyboard::Key::num5},
            {"Digit6", Keyboard::Key::num6},
            {"Digit7", Keyboard::Key::num7},
            {"Digit8", Keyboard::Key::num8},
            {"Digit9", Keyboard::Key::num9},
            {"KeyA", Keyboard::Key::a},
            {"KeyB", Keyboard::Key::b},
            {"KeyC", Keyboard::Key::c},
            {"KeyD", Keyboard::Key::d},
            {"KeyE", Keyboard::Key::e},
            {"KeyF", Keyboard::Key::f},
            {"KeyG", Keyboard::Key::g},
            {"KeyH", Keyboard::Key::h},
            {"KeyI", Keyboard::Key::i},
            {"KeyJ", Keyboard::Key::j},
            {"KeyK", Keyboard::Key::k},
            {"KeyL", Keyboard::Key::l},
            {"KeyM", Keyboard::Key::m},
            {"KeyN", Keyboard::Key::n},
            {"KeyO", Keyboard::Key::o},
            {"KeyP", Keyboard::Key::p},
            {"KeyQ", Keyboard::Key::q},
            {"KeyR", Keyboard::Key::r},
            {"KeyS", Keyboard::Key::s},
            {"KeyT", Keyboard::Key::t},
            {"KeyU", Keyboard::Key::u},
            {"KeyV", Keyboard::Key::v},
            {"KeyW", Keyboard::Key::w},
            {"KeyX", Keyboard::Key::x},
            {"KeyY", Keyboard::Key::y},
            {"KeyZ", Keyboard::Key::z},
            {"OSLeft", Keyboard::Key::leftSuper},
            {"OSRight", Keyboard::Key::rightSuper},
            {"Delete", Keyboard::Key::del},
            {"NumLock", Keyboard::Key::numLock},
            {"NumpadEnter", Keyboard::Key::numpadEnter},
            {"Numpad0", Keyboard::Key::numpad0},
            {"Numpad1", Keyboard::Key::numpad1},
            {"Numpad2", Keyboard::Key::numpad2},
            {"Numpad3", Keyboard::Key::numpad3},
            {"Numpad4", Keyboard::Key::numpad4},
            {"Numpad5", Keyboard::Key::numpad5},
            {"Numpad6", Keyboard::Key::numpad6},
            {"Numpad7", Keyboard::Key::numpad7},
            {"Numpad8", Keyboard::Key::numpad8},
            {"Numpad9", Keyboard::Key::numpad9},
            {"NumpadMultiply", Keyboard::Key::numpadMultiply},
            {"NumpadAdd", Keyboard::Key::numpadPlus},
            {"NumpadSubtract", Keyboard::Key::numpadMinus},
            {"NumpadComma", Keyboard::Key::numpadSeparator},
            {"NumpadDecimal", Keyboard::Key::numpadDecimal},
            {"NumpadDivide", Keyboard::Key::numpadDivide},
            {"NumpadEqual", Keyboard::Key::numpadEqual},
            {"F1", Keyboard::Key::f1},
            {"F2", Keyboard::Key::f2},
            {"F3", Keyboard::Key::f3},
            {"F4", Keyboard::Key::f4},
            {"F5", Keyboard::Key::f5},
            {"F6", Keyboard::Key::f6},
            {"F7", Keyboard::Key::f7},
            {"F8", Keyboard::Key::f8},
            {"F9", Keyboard::Key::f9},
            {"F10", Keyboard::Key::f10},
            {"F11", Keyboard::Key::f11},
            {"F12", Keyboard::Key::f12},
            {"F13", Keyboard::Key::f13},
            {"F14", Keyboard::Key::f14},
            {"F15", Keyboard::Key::f15},
            {"F16", Keyboard::Key::f16},
            {"F17", Keyboard::Key::f17},
            {"F18", Keyboard::Key::f18},
            {"F19", Keyboard::Key::f19},
            {"F20", Keyboard::Key::f20},
            {"F21", Keyboard::Key::f21},
            {"F22", Keyboard::Key::f22},
            {"F23", Keyboard::Key::f23},
            {"F24", Keyboard::Key::f24},
            {"ScrollLock", Keyboard::Key::scrollLock},
            {"Semicolon", Keyboard::Key::semicolon},
            {"Equal", Keyboard::Key::equal},
            {"Comma", Keyboard::Key::comma},
            {"Minus", Keyboard::Key::minus},
            {"Period", Keyboard::Key::period},
            {"Slash", Keyboard::Key::slash},
            {"Backquote", Keyboard::Key::grave},
            {"BracketLeft", Keyboard::Key::leftBracket},
            {"Backslash", Keyboard::Key::backslash},
            {"BracketRight", Keyboard::Key::rightBracket},
            {"Quote", Keyboard::Key::quote},
            {"IntlBackslash", Keyboard::Key::intlBackslash},
            {"PrintScreen", Keyboard::Key::printScreen},
            {"IntlRo", Keyboard::Key::ro},
            {"IntlYen", Keyboard::Key::yen}
        };

        Keyboard::Key convertKeyCode(const EM_UTF8 key[32])
        {
            auto i = keyMap.find(key);

            if (i != keyMap.end())
                return i->second;
            else
                return Keyboard::Key::none;
        }

        EM_BOOL emKeyCallback(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData)
        {
            auto keyboardDevice = static_cast<KeyboardDevice*>(userData);

            switch (eventType)
            {
                case EMSCRIPTEN_EVENT_KEYPRESS:
                case EMSCRIPTEN_EVENT_KEYDOWN:
                    keyboardDevice->handleKeyPress(convertKeyCode(keyEvent->code));
                    return EM_TRUE;
                case EMSCRIPTEN_EVENT_KEYUP:
                    keyboardDevice->handleKeyRelease(convertKeyCode(keyEvent->code));
                    return EM_TRUE;
            }

            return EM_FALSE;
        }

        EM_BOOL emMouseCallback(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData)
        {
            auto mouseDevice = static_cast<MouseDevice*>(userData);
            Mouse::Button button;

            switch (mouseEvent->button)
            {
                case 0:
                    button = Mouse::Button::left;
                    break;
                case 1:
                    button = Mouse::Button::middle;
                    break;
                case 2:
                    button = Mouse::Button::right;
                    break;
                default:
                    button = Mouse::Button::none;
                    break;
            }

            const ouzel::Vector2F position{
                static_cast<float>(mouseEvent->canvasX),
                static_cast<float>(mouseEvent->canvasY)
            };

            switch (eventType)
            {
                case EMSCRIPTEN_EVENT_MOUSEDOWN:
                    mouseDevice->handleButtonPress(button,
                                                   ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position));
                    return EM_TRUE;
                case EMSCRIPTEN_EVENT_MOUSEUP:
                    mouseDevice->handleButtonRelease(button,
                                                     ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position));
                    return EM_TRUE;
                case EMSCRIPTEN_EVENT_MOUSEMOVE:
                    mouseDevice->handleMove(ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position));
                    return EM_TRUE;
            }

            return EM_FALSE;
        }

        EM_BOOL emWheelCallback(int eventType, const EmscriptenWheelEvent* wheelEvent, void* userData)
        {
            auto mouseDevice = static_cast<MouseDevice*>(userData);

            if (eventType == EMSCRIPTEN_EVENT_WHEEL)
            {
                const ouzel::Vector2F position{
                    static_cast<float>(wheelEvent->mouse.canvasX),
                    static_cast<float>(wheelEvent->mouse.canvasY)
                };

                mouseDevice->handleScroll(ouzel::Vector2F(static_cast<float>(wheelEvent->deltaX), static_cast<float>(wheelEvent->deltaY)),
                                          ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position));

                return EM_TRUE;
            }

            return EM_FALSE;
        }

        EM_BOOL emPointerLockChangeCallback(int eventType, const EmscriptenPointerlockChangeEvent* pointerlockChangeEvent, void* userData)
        {
            auto mouseDevice = static_cast<MouseDevice*>(userData);

            if (eventType == EMSCRIPTEN_EVENT_POINTERLOCKCHANGE)
            {
                mouseDevice->handleCursorLockChange(pointerlockChangeEvent->isActive);
                return EM_TRUE;
            }

            return EM_FALSE;
        }

        EM_BOOL emGamepadCallback(int eventType, const EmscriptenGamepadEvent* gamepadEvent, void* userData)
        {
            auto inputEm = static_cast<InputSystem*>(userData);

            if (eventType == EMSCRIPTEN_EVENT_GAMEPADCONNECTED)
            {
                inputEm->handleGamepadConnected(gamepadEvent->index);
                return EM_TRUE;
            }
            else if (eventType == EMSCRIPTEN_EVENT_GAMEPADDISCONNECTED)
            {
                inputEm->handleGamepadDisconnected(gamepadEvent->index);
                return EM_TRUE;
            }

            return EM_FALSE;
        }

        EM_BOOL emTouchCallback(int eventType, const EmscriptenTouchEvent* touchEvent, void* userData)
        {
            auto touchpadDevice = static_cast<TouchpadDevice*>(userData);

            for (int i = 0; i < touchEvent->numTouches; ++i)
            {
                if (touchEvent->touches[i].isChanged)
                {
                    const ouzel::Vector2F position{
                        static_cast<float>(touchEvent->touches[i].canvasX),
                        static_cast<float>(touchEvent->touches[i].canvasY)
                    };

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

            return EM_FALSE;
        }
    }

    InputSystem::InputSystem(const std::function<std::future<bool>(const Event&)>& initCallback):
        input::InputSystem(initCallback),
        keyboardDevice(std::make_unique<KeyboardDevice>(*this, getNextDeviceId())),
        mouseDevice(std::make_unique<MouseDevice>(*this, getNextDeviceId())),
        touchpadDevice(std::make_unique<TouchpadDevice>(*this, getNextDeviceId(), true))
    {
        emscripten_set_keypress_callback(nullptr, keyboardDevice.get(), EM_TRUE, emKeyCallback);
        emscripten_set_keydown_callback(nullptr, keyboardDevice.get(), EM_TRUE, emKeyCallback);
        emscripten_set_keyup_callback(nullptr, keyboardDevice.get(), EM_TRUE, emKeyCallback);

        emscripten_set_mousedown_callback("#canvas", mouseDevice.get(), EM_TRUE, emMouseCallback);
        emscripten_set_mouseup_callback("#canvas", mouseDevice.get(), EM_TRUE, emMouseCallback);
        emscripten_set_mousemove_callback("#canvas", mouseDevice.get(), EM_TRUE, emMouseCallback);
        emscripten_set_wheel_callback("#canvas", mouseDevice.get(), EM_TRUE, emWheelCallback);
        emscripten_set_pointerlockchange_callback(nullptr, mouseDevice.get(), EM_TRUE, emPointerLockChangeCallback);

        emscripten_set_gamepadconnected_callback(this, EM_TRUE, emGamepadCallback);
        emscripten_set_gamepaddisconnected_callback(this, EM_TRUE, emGamepadCallback);

        emscripten_set_touchstart_callback("#canvas", touchpadDevice.get(), EM_TRUE, emTouchCallback);
        emscripten_set_touchend_callback("#canvas", touchpadDevice.get(), EM_TRUE, emTouchCallback);
        emscripten_set_touchmove_callback("#canvas", touchpadDevice.get(), EM_TRUE, emTouchCallback);
        emscripten_set_touchcancel_callback("#canvas", touchpadDevice.get(), EM_TRUE, emTouchCallback);

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
