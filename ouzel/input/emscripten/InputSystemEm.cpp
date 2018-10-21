// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <map>
#include <emscripten.h>
#include "InputSystemEm.hpp"
#include "GamepadDeviceEm.hpp"
#include "core/Engine.hpp"
#include "utils/Log.hpp"

static std::map<std::string, ouzel::input::Keyboard::Key> keyMap = {
    {"Backspace", ouzel::input::Keyboard::Key::BACKSPACE},
    {"Tab", ouzel::input::Keyboard::Key::TAB},
    {"Enter", ouzel::input::Keyboard::Key::ENTER},
    {"ShiftLeft", ouzel::input::Keyboard::Key::LEFT_SHIFT},
    {"ShiftRight", ouzel::input::Keyboard::Key::RIGHT_SHIFT},
    {"ControlLeft", ouzel::input::Keyboard::Key::LEFT_CONTROL},
    {"ControlRight", ouzel::input::Keyboard::Key::RIGHT_CONTROL},
    {"AltLeft", ouzel::input::Keyboard::Key::LEFT_ALT},
    {"AltRight", ouzel::input::Keyboard::Key::RIGHT_ALT},
    {"MetaLeft", ouzel::input::Keyboard::Key::LEFT_SUPER},
    {"MetaRight", ouzel::input::Keyboard::Key::RIGHT_SUPER},
    {"Pause", ouzel::input::Keyboard::Key::PAUSE},
    {"CapsLock", ouzel::input::Keyboard::Key::CAPS_LOCK},
    {"Escape", ouzel::input::Keyboard::Key::ESCAPE},
    {"PageUp", ouzel::input::Keyboard::Key::PAGE_UP},
    {"PageDown", ouzel::input::Keyboard::Key::PAGE_DOWN},
    {"ContextMenu", ouzel::input::Keyboard::Key::MENU},
    {"Help", ouzel::input::Keyboard::Key::HELP},
    {"Home", ouzel::input::Keyboard::Key::HOME},
    {"End", ouzel::input::Keyboard::Key::END},
    {"ArrowLeft", ouzel::input::Keyboard::Key::LEFT},
    {"ArrowUp", ouzel::input::Keyboard::Key::UP},
    {"ArrowRight", ouzel::input::Keyboard::Key::RIGHT},
    {"ArrowDown", ouzel::input::Keyboard::Key::DOWN},
    {"Insert", ouzel::input::Keyboard::Key::INSERT},
    {"Delete", ouzel::input::Keyboard::Key::DEL},
    {"Digit0", ouzel::input::Keyboard::Key::NUM_0},
    {"Digit1", ouzel::input::Keyboard::Key::NUM_1},
    {"Digit2", ouzel::input::Keyboard::Key::NUM_2},
    {"Digit3", ouzel::input::Keyboard::Key::NUM_3},
    {"Digit4", ouzel::input::Keyboard::Key::NUM_4},
    {"Digit5", ouzel::input::Keyboard::Key::NUM_5},
    {"Digit6", ouzel::input::Keyboard::Key::NUM_6},
    {"Digit7", ouzel::input::Keyboard::Key::NUM_7},
    {"Digit8", ouzel::input::Keyboard::Key::NUM_8},
    {"Digit9", ouzel::input::Keyboard::Key::NUM_9},
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
    {"OSLeft", ouzel::input::Keyboard::Key::LEFT_SUPER},
    {"OSRight", ouzel::input::Keyboard::Key::RIGHT_SUPER},
    {"Delete", ouzel::input::Keyboard::Key::DEL},
    {"NumLock", ouzel::input::Keyboard::Key::NUM_LOCK},
    {"NumpadEnter", ouzel::input::Keyboard::Key::NUMPAD_ENTER},
    {"Numpad0", ouzel::input::Keyboard::Key::NUMPAD_0},
    {"Numpad1", ouzel::input::Keyboard::Key::NUMPAD_1},
    {"Numpad2", ouzel::input::Keyboard::Key::NUMPAD_2},
    {"Numpad3", ouzel::input::Keyboard::Key::NUMPAD_3},
    {"Numpad4", ouzel::input::Keyboard::Key::NUMPAD_4},
    {"Numpad5", ouzel::input::Keyboard::Key::NUMPAD_5},
    {"Numpad6", ouzel::input::Keyboard::Key::NUMPAD_6},
    {"Numpad7", ouzel::input::Keyboard::Key::NUMPAD_7},
    {"Numpad8", ouzel::input::Keyboard::Key::NUMPAD_8},
    {"Numpad9", ouzel::input::Keyboard::Key::NUMPAD_9},
    {"NumpadMultiply", ouzel::input::Keyboard::Key::NUMPAD_MULTIPLY},
    {"NumpadAdd", ouzel::input::Keyboard::Key::NUMPAD_PLUS},
    {"NumpadSubtract", ouzel::input::Keyboard::Key::NUMPAD_MINUS},
    {"NumpadComma", ouzel::input::Keyboard::Key::NUMPAD_SEPARATOR},
    {"NumpadDecimal", ouzel::input::Keyboard::Key::NUMPAD_DECIMAL},
    {"NumpadDivide", ouzel::input::Keyboard::Key::NUMPAD_DIVIDE},
    {"NumpadEqual", ouzel::input::Keyboard::Key::NUMPAD_EQUAL},
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
    {"ScrollLock", ouzel::input::Keyboard::Key::SCROLL_LOCK},
    {"Semicolon", ouzel::input::Keyboard::Key::SEMICOLON},
    {"Equal", ouzel::input::Keyboard::Key::EQUAL},
    {"Comma", ouzel::input::Keyboard::Key::COMMA},
    {"Minus", ouzel::input::Keyboard::Key::MINUS},
    {"Period", ouzel::input::Keyboard::Key::PERIOD},
    {"Slash", ouzel::input::Keyboard::Key::SLASH},
    {"Backquote", ouzel::input::Keyboard::Key::GRAVE},
    {"BracketLeft", ouzel::input::Keyboard::Key::LEFT_BRACKET},
    {"Backslash", ouzel::input::Keyboard::Key::BACKSLASH},
    {"BracketRight", ouzel::input::Keyboard::Key::RIGHT_BRACKET},
    {"Quote", ouzel::input::Keyboard::Key::QUOTE},
    {"IntlBackslash", ouzel::input::Keyboard::Key::INTL_BACKSLASH},
    {"PrintScreen", ouzel::input::Keyboard::Key::PRINT_SCREEN},
    {"IntlRo", ouzel::input::Keyboard::Key::RO},
    {"IntlYen", ouzel::input::Keyboard::Key::YEN}
};

static ouzel::input::Keyboard::Key convertKeyCode(const EM_UTF8 key[32])
{
    auto i = keyMap.find(key);

    if (i != keyMap.end())
        return i->second;
    else
        return ouzel::input::Keyboard::Key::NONE;
}

static EM_BOOL emKeyCallback(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData)
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

static EM_BOOL emWheelCallback(int eventType, const EmscriptenWheelEvent* wheelEvent, void* userData)
{
    ouzel::input::MouseDevice* mouseDevice = static_cast<ouzel::input::MouseDevice*>(userData);

    if (eventType == EMSCRIPTEN_EVENT_WHEEL)
    {
        ouzel::Vector2 position(static_cast<float>(wheelEvent->mouse.canvasX),
                                static_cast<float>(wheelEvent->mouse.canvasY));

        mouseDevice->handleScroll(ouzel::Vector2(static_cast<float>(wheelEvent->deltaX), static_cast<float>(wheelEvent->deltaY)),
                                  ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position));

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

static EM_BOOL emTouchCallback(int eventType, const EmscriptenTouchEvent* touchEvent, void* userData)
{
    ouzel::input::TouchpadDevice* touchpadDevice = static_cast<ouzel::input::TouchpadDevice*>(userData);

    for (int i = 0; i < touchEvent->numTouches; ++i)
    {
        if (touchEvent->touches[i].isChanged)
        {
            ouzel::Vector2 position(static_cast<float>(touchEvent->touches[i].canvasX),
                                    static_cast<float>(touchEvent->touches[i].canvasY));

            switch (eventType)
            {
                case EMSCRIPTEN_EVENT_TOUCHSTART:
                    touchpadDevice->handleTouchBegin(static_cast<uint64_t>(touchEvent->touches[i].identifier),
                                                     ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position),
                                                     1.0F);
                    break;
                case EMSCRIPTEN_EVENT_TOUCHEND:
                    touchpadDevice->handleTouchEnd(static_cast<uint64_t>(touchEvent->touches[i].identifier),
                                                   ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position),
                                                   1.0F);
                    break;
                case EMSCRIPTEN_EVENT_TOUCHMOVE:
                    touchpadDevice->handleTouchMove(static_cast<uint64_t>(touchEvent->touches[i].identifier),
                                                    ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position),
                                                    1.0F);
                    break;
                case EMSCRIPTEN_EVENT_TOUCHCANCEL:
                    touchpadDevice->handleTouchCancel(static_cast<uint64_t>(touchEvent->touches[i].identifier),
                                                      ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position),
                                                      1.0F);
                    break;
            }
        }
    }

    return false;
}

namespace ouzel
{
    namespace input
    {
        InputSystemEm::InputSystemEm(EventHandler& initEventHandler):
            InputSystem(initEventHandler),
            keyboardDevice(new KeyboardDevice(*this, ++lastDeviceId)),
            mouseDevice(new MouseDeviceEm(*this, ++lastDeviceId)),
            touchpadDevice(new TouchpadDevice(*this, ++lastDeviceId))
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

            int result = emscripten_get_num_gamepads();

            if (result == EMSCRIPTEN_RESULT_NOT_SUPPORTED)
                Log(Log::Level::INFO) << "Gamepads not supported";
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
                case Command::Type::SET_PLAYER_INDEX:
                {
                    break;
                }
                case Command::Type::SET_VIBRATION:
                {
                    break;
                }
                case Command::Type::SET_POSITION:
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
                        if (inputDevice == mouseDevice.get())
                            mouseDevice->setCursorVisible(command.locked);
                    }
                    break;
                }
                case Command::Type::SET_CURSOR_LOCKED:
                {
                    if (InputDevice* inputDevice = getInputDevice(command.deviceId))
                    {
                        if (inputDevice == mouseDevice.get())
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
                GamepadDeviceEm* gamepadDevice = static_cast<GamepadDeviceEm*>(i.second.get());
                gamepadDevice->update();
            }
        }

        void InputSystemEm::handleGamepadConnected(long index)
        {
            std::unique_ptr<GamepadDeviceEm> gamepadDevice(new GamepadDeviceEm(*this, ++lastDeviceId, index));
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
