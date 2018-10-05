// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <map>
#include <emscripten.h>
#include "InputSystemEm.hpp"
#include "GamepadDeviceEm.hpp"
#include "core/Engine.hpp"
#include "utils/Log.hpp"

static EM_BOOL emKeyCallback(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData)
{
    ouzel::input::KeyboardDevice* keyboardDevice = static_cast<ouzel::input::KeyboardDevice*>(userData);

    switch (eventType)
    {
        case EMSCRIPTEN_EVENT_KEYPRESS:
        case EMSCRIPTEN_EVENT_KEYDOWN:
            keyboardDevice->handleKeyPress(ouzel::input::InputSystemEm::convertKeyCode(keyEvent->code),
                                           ouzel::input::InputSystemEm::getKeyboardModifiers(keyEvent));
            return true;
        case EMSCRIPTEN_EVENT_KEYUP:
            keyboardDevice->handleKeyRelease(ouzel::input::InputSystemEm::convertKeyCode(keyEvent->code),
                                             ouzel::input::InputSystemEm::getKeyboardModifiers(keyEvent));
            return true;
    }

    return false;
}

static EM_BOOL emMouseCallback(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData)
{
    ouzel::input::MouseDevice* mouseDevice = static_cast<ouzel::input::MouseDevice*>(userData);
    ouzel::input::Mouse::Button button;

    switch (mouseEvent->button)
    {
        case 0:
            button = ouzel::input::Mouse::Button::LEFT;
            break;
        case 1:
            button = ouzel::input::Mouse::Button::RIGHT;
            break;
        case 2:
            button = ouzel::input::Mouse::Button::MIDDLE;
            break;
        default:
            button = ouzel::input::Mouse::Button::NONE;
            break;
    }

    ouzel::Vector2 position(static_cast<float>(mouseEvent->canvasX),
                            static_cast<float>(mouseEvent->canvasY));

    switch (eventType)
    {
        case EMSCRIPTEN_EVENT_MOUSEDOWN:
            mouseDevice->handleButtonPress(button,
                                           ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position),
                                           ouzel::input::InputSystemEm::getMouseModifiers(mouseEvent));
            return true;
        case EMSCRIPTEN_EVENT_MOUSEUP:
            mouseDevice->handleButtonRelease(button,
                                             ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position),
                                             ouzel::input::InputSystemEm::getMouseModifiers(mouseEvent));
            return true;
        case EMSCRIPTEN_EVENT_MOUSEMOVE:
            mouseDevice->handleMove(ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position),
                                    ouzel::input::InputSystemEm::getMouseModifiers(mouseEvent));
            return true;
    }

    return false;
}

static EM_BOOL emWheelCallback(int eventType, const EmscriptenWheelEvent* wheelEvent, void* userData)
{
    ouzel::input::MouseDevice* mouseDevice = static_cast<ouzel::input::MouseDevice*>(userData);

    if (eventType == EMSCRIPTEN_EVENT_WHEEL)
    {
        ouzel::Vector2 position(static_cast<float>(wheelEvent->mouse.canvasX),
                                static_cast<float>(wheelEvent->mouse.canvasY));

        mouseDevice->handleScroll(ouzel::Vector2(static_cast<float>(wheelEvent->deltaX), static_cast<float>(wheelEvent->deltaY)),
                                  ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position),
                                  ouzel::input::InputSystemEm::getMouseModifiers(&wheelEvent->mouse));

        return true;
    }

    return false;
}

static EM_BOOL emPointerLockChangeCallback(int eventType, const EmscriptenPointerlockChangeEvent* pointerlockChangeEvent, void* userData)
{
    ouzel::input::MouseDevice* mouseDevice = static_cast<ouzel::input::MouseDevice*>(userData);

    if (eventType == EMSCRIPTEN_EVENT_POINTERLOCKCHANGE)
    {
        mouseDevice->handleCursorLockChange(pointerlockChangeEvent->isActive);
        return true;
    }

    return false;
}

static EM_BOOL emGamepadCallback(int eventType, const EmscriptenGamepadEvent* gamepadEvent, void* userData)
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

static EM_BOOL emTouchCallback(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData)
{
    ouzel::input::TouchpadDevice* touchpadDevice = static_cast<ouzel::input::TouchpadDevice*>(userData);

    switch (eventType)
    {
        case EMSCRIPTEN_EVENT_TOUCHSTART:
            // TODO: implement
            break;
        case EMSCRIPTEN_EVENT_TOUCHEND:
            // TODO: implement
            break;
        case EMSCRIPTEN_EVENT_TOUCHMOVE:
            // TODO: implement
            break;
        case EMSCRIPTEN_EVENT_TOUCHCANCEL:
            // TODO: implement
            break;
    }

    return false;
}

namespace ouzel
{
    namespace input
    {
        static std::map<std::string, Keyboard::Key> keyMap = {
            {"Backspace", Keyboard::Key::BACKSPACE},
            {"Tab", Keyboard::Key::TAB},
            {"Enter", Keyboard::Key::RETURN},
            {"ShiftLeft", Keyboard::Key::LEFT_SHIFT},
            {"ShiftRight", Keyboard::Key::RIGHT_SHIFT},
            {"ControlLeft", Keyboard::Key::LEFT_CONTROL},
            {"ControlRight", Keyboard::Key::RIGHT_CONTROL},
            {"AltLeft", Keyboard::Key::LEFT_ALT},
            {"AltRight", Keyboard::Key::RIGHT_ALT},
            {"MetaLeft", Keyboard::Key::LEFT_SUPER},
            {"MetaRight", Keyboard::Key::RIGHT_SUPER},
            {"Pause", Keyboard::Key::PAUSE},
            {"CapsLock", Keyboard::Key::CAPITAL},
            {"Escape", Keyboard::Key::ESCAPE},
            {"PageUp", Keyboard::Key::PRIOR},
            {"PageDown", Keyboard::Key::NEXT},
            {"Home", Keyboard::Key::HOME},
            {"End", Keyboard::Key::END},
            {"ArrowLeft", Keyboard::Key::LEFT},
            {"ArrowUp", Keyboard::Key::UP},
            {"ArrowRight", Keyboard::Key::RIGHT},
            {"ArrowDown", Keyboard::Key::DOWN},
            {"Insert", Keyboard::Key::INSERT},
            {"Delete", Keyboard::Key::DEL},
            {"Digit0", Keyboard::Key::NUM_0},
            {"Digit1", Keyboard::Key::NUM_1},
            {"Digit2", Keyboard::Key::NUM_2},
            {"Digit3", Keyboard::Key::NUM_3},
            {"Digit4", Keyboard::Key::NUM_4},
            {"Digit5", Keyboard::Key::NUM_5},
            {"Digit6", Keyboard::Key::NUM_6},
            {"Digit7", Keyboard::Key::NUM_7},
            {"Digit8", Keyboard::Key::NUM_8},
            {"Digit9", Keyboard::Key::NUM_9},
            {"KeyA", Keyboard::Key::A},
            {"KeyB", Keyboard::Key::B},
            {"KeyC", Keyboard::Key::C},
            {"KeyD", Keyboard::Key::D},
            {"KeyE", Keyboard::Key::E},
            {"KeyF", Keyboard::Key::F},
            {"KeyG", Keyboard::Key::G},
            {"KeyH", Keyboard::Key::H},
            {"KeyI", Keyboard::Key::I},
            {"KeyJ", Keyboard::Key::J},
            {"KeyK", Keyboard::Key::K},
            {"KeyL", Keyboard::Key::L},
            {"KeyM", Keyboard::Key::M},
            {"KeyN", Keyboard::Key::N},
            {"KeyO", Keyboard::Key::O},
            {"KeyP", Keyboard::Key::P},
            {"KeyQ", Keyboard::Key::Q},
            {"KeyR", Keyboard::Key::R},
            {"KeyS", Keyboard::Key::S},
            {"KeyT", Keyboard::Key::T},
            {"KeyU", Keyboard::Key::U},
            {"KeyV", Keyboard::Key::V},
            {"KeyW", Keyboard::Key::W},
            {"KeyX", Keyboard::Key::X},
            {"KeyY", Keyboard::Key::Y},
            {"KeyZ", Keyboard::Key::Z},
            {"OSLeft", Keyboard::Key::LEFT_SUPER},
            {"OSRight", Keyboard::Key::RIGHT_SUPER},
            {"Delete", Keyboard::Key::DEL},
            {"NumpadEnter", Keyboard::Key::SEPARATOR},
            {"NumpadDigit0", Keyboard::Key::NUMPAD_0},
            {"NumpadDigit1", Keyboard::Key::NUMPAD_1},
            {"NumpadDigit2", Keyboard::Key::NUMPAD_2},
            {"NumpadDigit3", Keyboard::Key::NUMPAD_3},
            {"NumpadDigit4", Keyboard::Key::NUMPAD_4},
            {"NumpadDigit5", Keyboard::Key::NUMPAD_5},
            {"NumpadDigit6", Keyboard::Key::NUMPAD_6},
            {"NumpadDigit7", Keyboard::Key::NUMPAD_7},
            {"NumpadDigit8", Keyboard::Key::NUMPAD_8},
            {"NumpadDigit9", Keyboard::Key::NUMPAD_9},
            {"NumpadMultiply", Keyboard::Key::MULTIPLY},
            {"NumpadAdd", Keyboard::Key::ADD},
            {"NumpadSubtract", Keyboard::Key::SUBTRACT},
            {"NumpadDecimal", Keyboard::Key::DECIMAL},
            {"NumpadDivide", Keyboard::Key::DIVIDE},
            {"F1", Keyboard::Key::F1},
            {"F2", Keyboard::Key::F2},
            {"F3", Keyboard::Key::F3},
            {"F4", Keyboard::Key::F4},
            {"F5", Keyboard::Key::F5},
            {"F6", Keyboard::Key::F6},
            {"F7", Keyboard::Key::F7},
            {"F8", Keyboard::Key::F8},
            {"F9", Keyboard::Key::F9},
            {"F10", Keyboard::Key::F10},
            {"F11", Keyboard::Key::F11},
            {"F12", Keyboard::Key::F12},
            {"NumLock", Keyboard::Key::NUMLOCK},
            {"ScrollLock", Keyboard::Key::SCROLL},
            {"Semicolon", Keyboard::Key::SEMICOLON},
            {"Equal", Keyboard::Key::EQUAL},
            {"Comma", Keyboard::Key::COMMA},
            {"Minus", Keyboard::Key::MINUS},
            {"Period", Keyboard::Key::PERIOD},
            {"Slash", Keyboard::Key::SLASH},
            {"Backquote", Keyboard::Key::GRAVE},
            {"BracketLeft", Keyboard::Key::LEFT_BRACKET},
            {"Backslash", Keyboard::Key::BACKSLASH},
            {"BracketRight", Keyboard::Key::RIGHT_BRACKET},
            {"Quote", Keyboard::Key::QUOTE},
            {"IntlBackslash", Keyboard::Key::LESS}
        };

        Keyboard::Key InputSystemEm::convertKeyCode(const EM_UTF8 key[32])
        {
            auto i = keyMap.find(key);

            if (i != keyMap.end())
                return i->second;
            else
                return Keyboard::Key::NONE;
        }

        uint32_t InputSystemEm::getKeyboardModifiers(const EmscriptenKeyboardEvent* keyboardEvent)
        {
            uint32_t modifiers = 0;

            if (keyboardEvent->ctrlKey) modifiers |= ouzel::CONTROL_DOWN;
            if (keyboardEvent->shiftKey) modifiers |= ouzel::SHIFT_DOWN;
            if (keyboardEvent->altKey) modifiers |= ouzel::ALT_DOWN;
            if (keyboardEvent->metaKey) modifiers |= ouzel::SUPER_DOWN;

            return modifiers;
        }

        uint32_t InputSystemEm::getMouseModifiers(const EmscriptenMouseEvent* mouseEvent)
        {
            uint32_t modifiers = 0;

            if (mouseEvent->ctrlKey) modifiers |= ouzel::CONTROL_DOWN;
            if (mouseEvent->shiftKey) modifiers |= ouzel::SHIFT_DOWN;
            if (mouseEvent->altKey) modifiers |= ouzel::ALT_DOWN;
            if (mouseEvent->metaKey) modifiers |= ouzel::SUPER_DOWN;

            if (mouseEvent->buttons & (1 << 0)) modifiers |= LEFT_MOUSE_DOWN;
            if (mouseEvent->buttons & (1 << 1)) modifiers |= RIGHT_MOUSE_DOWN;
            if (mouseEvent->buttons & (1 << 2)) modifiers |= MIDDLE_MOUSE_DOWN;

            return modifiers;
        }

        InputSystemEm::InputSystemEm()
        {
            std::unique_ptr<KeyboardDevice> keyboard(new KeyboardDevice(*this, ++lastDeviceId));
            keyboardDevice = keyboard.get();
            addInputDevice(std::move(keyboard));

            std::unique_ptr<MouseDeviceEm> mouse(new MouseDeviceEm(*this, ++lastDeviceId));
            mouseDevice = mouse.get();
            addInputDevice(std::move(mouse));

            std::unique_ptr<TouchpadDevice> touchpad(new TouchpadDevice(*this, ++lastDeviceId));
            touchpadDevice = touchpad.get();
            addInputDevice(std::move(touchpad));

            emscripten_set_keypress_callback(nullptr, keyboardDevice, true, emKeyCallback);
            emscripten_set_keydown_callback(nullptr, keyboardDevice, true, emKeyCallback);
            emscripten_set_keyup_callback(nullptr, keyboardDevice, true, emKeyCallback);

            emscripten_set_mousedown_callback("#canvas", mouseDevice, true, emMouseCallback);
            emscripten_set_mouseup_callback("#canvas", mouseDevice, true, emMouseCallback);
            emscripten_set_mousemove_callback("#canvas", mouseDevice, true, emMouseCallback);
            emscripten_set_wheel_callback("#canvas", mouseDevice, true, emWheelCallback);
            emscripten_set_pointerlockchange_callback(nullptr, mouseDevice, true, emPointerLockChangeCallback);

            emscripten_set_gamepadconnected_callback(this, true, emGamepadCallback);
            emscripten_set_gamepaddisconnected_callback(this, true, emGamepadCallback);

            emscripten_set_touchstart_callback("#canvas", touchpadDevice, true, emTouchCallback);
            emscripten_set_touchend_callback("#canvas", touchpadDevice, true, emTouchCallback);
            emscripten_set_touchmove_callback("#canvas", touchpadDevice, true, emTouchCallback);
            emscripten_set_touchcancel_callback("#canvas", touchpadDevice, true, emTouchCallback);

            int result = emscripten_get_num_gamepads();

            if (result == EMSCRIPTEN_RESULT_NOT_SUPPORTED)
                Log(Log::Level::INFO) << "Gamepads not supported";
            else
            {
                for (long index = 0; index < result; ++index)
                    handleGamepadConnected(index);
            }
        }

        void InputSystemEm::executeCommand(Command command)
        {
            switch (command.type)
            {
                case Command::Type::START_DEVICE_DISCOVERY:
                    break;
                case Command::Type::STOP_DEVICE_DISCOVERY:
                    break;
                case Command::Type::SET_PLAYER_INDEX:
                {
                    break;
                }
                case Command::Type::SET_VIBRATION:
                {
                    break;
                }
                case Command::Type::SET_POSITON:
                {
                    break;
                }
                case Command::Type::SET_CURSOR:
                {
                    break;
                }
                case Command::Type::SET_CURSOR_VISIBLE:
                {
                    if (InputDevice* inputDevice = getInputDevice(command.deviceId))
                    {
                        if (inputDevice == mouseDevice)
                            mouseDevice->setCursorVisible(command.locked);
                    }
                    break;
                }
                case Command::Type::SET_CURSOR_LOCKED:
                {
                    if (InputDevice* inputDevice = getInputDevice(command.deviceId))
                    {
                        if (inputDevices == mouseDevice)
                            mouseDevice->setCursorLocked(command.locked);
                    }
                    break;
                }
                case Command::Type::SHOW_VIRTUAL_KEYBOARD:
                    break;
                case Command::Type::HIDE_VIRTUAL_KEYBOARD:
                    break;
                default:
                    break;
            }
        }

        void InputSystemEm::update()
        {
            for (const auto& i : gamepadDevices)
            {
                GamepadDeviceEm* gamepadDevice = static_cast<GamepadDeviceEm*>(i.second);
                gamepadDevice->update();
            }
        }

        void InputSystemEm::handleGamepadConnected(long index)
        {
            std::unique_ptr<GamepadDeviceEm> gamepadDevice(new GamepadDeviceEm(*this, ++lastDeviceId, index));
            gamepadDevices.insert(std::make_pair(index, gamepadDevice.get()));
            addInputDevice(std::move(gamepadDevice));
        }

        void InputSystemEm::handleGamepadDisconnected(long index)
        {
            auto i = gamepadDevices.find(index);

            if (i != gamepadDevices.end())
            {
                removeInputDevice(i->second);
                gamepadDevices.erase(i);
            }
        }
    } // namespace input
} // namespace ouzel
