// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <emscripten.h>
#include "InputManagerEm.hpp"
#include "GamepadEm.hpp"
#include "core/Engine.hpp"
#include "core/Window.hpp"
#include "events/Event.hpp"
#include "utils/Log.hpp"

static EM_BOOL emKeyCallback(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData)
{
    ouzel::input::InputManagerEm* inputEm = static_cast<ouzel::input::InputManagerEm*>(userData);

    switch (eventType)
    {
        case EMSCRIPTEN_EVENT_KEYPRESS:
        case EMSCRIPTEN_EVENT_KEYDOWN:
            inputEm->keyPress(ouzel::input::InputManagerEm::convertKeyCode(keyEvent->code),
                              ouzel::input::InputManagerEm::getKeyboardModifiers(keyEvent));
            return true;
        case EMSCRIPTEN_EVENT_KEYUP:
            inputEm->keyRelease(ouzel::input::InputManagerEm::convertKeyCode(keyEvent->code),
                                ouzel::input::InputManagerEm::getKeyboardModifiers(keyEvent));
            return true;
    }

    return false;
}

static EM_BOOL emMouseCallback(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData)
{
    ouzel::input::InputManagerEm* inputEm = static_cast<ouzel::input::InputManagerEm*>(userData);
    ouzel::input::MouseButton button;

    switch (mouseEvent->button)
    {
        case 0:
            button = ouzel::input::MouseButton::LEFT;
            break;
        case 1:
            button = ouzel::input::MouseButton::RIGHT;
            break;
        case 2:
            button = ouzel::input::MouseButton::MIDDLE;
            break;
        default:
            button = ouzel::input::MouseButton::NONE;
            break;
    }

    ouzel::Vector2 position(static_cast<float>(mouseEvent->canvasX),
                            static_cast<float>(mouseEvent->canvasY));

    switch (eventType)
    {
        case EMSCRIPTEN_EVENT_MOUSEDOWN:
            inputEm->mouseButtonPress(button,
                                      ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position),
                                      ouzel::input::InputManagerEm::getMouseModifiers(mouseEvent));
            return true;
        case EMSCRIPTEN_EVENT_MOUSEUP:
            inputEm->mouseButtonRelease(button,
                                        ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position),
                                        ouzel::input::InputManagerEm::getMouseModifiers(mouseEvent));
            return true;
        case EMSCRIPTEN_EVENT_MOUSEMOVE:
            inputEm->mouseMove(ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position),
                               ouzel::input::InputManagerEm::getMouseModifiers(mouseEvent));
            return true;
    }

    return false;
}

static EM_BOOL emWheelCallback(int eventType, const EmscriptenWheelEvent* wheelEvent, void* userData)
{
    if (eventType == EMSCRIPTEN_EVENT_WHEEL)
    {
        ouzel::input::InputManagerEm* inputEm = static_cast<ouzel::input::InputManagerEm*>(userData);
        ouzel::Vector2 position(static_cast<float>(wheelEvent->mouse.canvasX),
                                static_cast<float>(wheelEvent->mouse.canvasY));

        inputEm->mouseScroll(ouzel::Vector2(static_cast<float>(wheelEvent->deltaX), static_cast<float>(wheelEvent->deltaY)),
                             ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position),
                             ouzel::input::InputManagerEm::getMouseModifiers(&wheelEvent->mouse));

        return true;
    }

    return false;
}

static EM_BOOL emPointerLockChangeCallback(int eventType, const EmscriptenPointerlockChangeEvent* pointerlockChangeEvent, void* userData)
{
    if (eventType == EMSCRIPTEN_EVENT_POINTERLOCKCHANGE)
    {
        ouzel::input::InputManagerEm* inputEm = static_cast<ouzel::input::InputManagerEm*>(userData);
        inputEm->pointerLockChanged(pointerlockChangeEvent->isActive);

        return true;
    }

    return false;
}

static EM_BOOL emGamepadCallback(int eventType, const EmscriptenGamepadEvent* gamepadEvent, void* userData)
{
    ouzel::input::InputManagerEm* inputEm = static_cast<ouzel::input::InputManagerEm*>(userData);

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

namespace ouzel
{
    namespace input
    {
        static std::map<std::string, KeyboardKey> keyMap = {
            {"Backspace", KeyboardKey::BACKSPACE},
            {"Tab", KeyboardKey::TAB},
            {"Enter", KeyboardKey::RETURN},
            {"ShiftLeft", KeyboardKey::LEFT_SHIFT},
            {"ShiftRight", KeyboardKey::RIGHT_SHIFT},
            {"ControlLeft", KeyboardKey::LEFT_CONTROL},
            {"ControlRight", KeyboardKey::RIGHT_CONTROL},
            {"AltLeft", KeyboardKey::LEFT_ALT},
            {"AltRight", KeyboardKey::RIGHT_ALT},
            {"MetaLeft", KeyboardKey::LEFT_SUPER},
            {"MetaRight", KeyboardKey::RIGHT_SUPER},
            {"Pause", KeyboardKey::PAUSE},
            {"CapsLock", KeyboardKey::CAPITAL},
            {"Escape", KeyboardKey::ESCAPE},
            {"PageUp", KeyboardKey::PRIOR},
            {"PageDown", KeyboardKey::NEXT},
            {"Home", KeyboardKey::HOME},
            {"End", KeyboardKey::END},
            {"ArrowLeft", KeyboardKey::LEFT},
            {"ArrowUp", KeyboardKey::UP},
            {"ArrowRight", KeyboardKey::RIGHT},
            {"ArrowDown", KeyboardKey::DOWN},
            {"Insert", KeyboardKey::INSERT},
            {"Delete", KeyboardKey::DEL},
            {"Digit0", KeyboardKey::NUM_0},
            {"Digit1", KeyboardKey::NUM_1},
            {"Digit2", KeyboardKey::NUM_2},
            {"Digit3", KeyboardKey::NUM_3},
            {"Digit4", KeyboardKey::NUM_4},
            {"Digit5", KeyboardKey::NUM_5},
            {"Digit6", KeyboardKey::NUM_6},
            {"Digit7", KeyboardKey::NUM_7},
            {"Digit8", KeyboardKey::NUM_8},
            {"Digit9", KeyboardKey::NUM_9},
            {"KeyA", KeyboardKey::A},
            {"KeyB", KeyboardKey::B},
            {"KeyC", KeyboardKey::C},
            {"KeyD", KeyboardKey::D},
            {"KeyE", KeyboardKey::E},
            {"KeyF", KeyboardKey::F},
            {"KeyG", KeyboardKey::G},
            {"KeyH", KeyboardKey::H},
            {"KeyI", KeyboardKey::I},
            {"KeyJ", KeyboardKey::J},
            {"KeyK", KeyboardKey::K},
            {"KeyL", KeyboardKey::L},
            {"KeyM", KeyboardKey::M},
            {"KeyN", KeyboardKey::N},
            {"KeyO", KeyboardKey::O},
            {"KeyP", KeyboardKey::P},
            {"KeyQ", KeyboardKey::Q},
            {"KeyR", KeyboardKey::R},
            {"KeyS", KeyboardKey::S},
            {"KeyT", KeyboardKey::T},
            {"KeyU", KeyboardKey::U},
            {"KeyV", KeyboardKey::V},
            {"KeyW", KeyboardKey::W},
            {"KeyX", KeyboardKey::X},
            {"KeyY", KeyboardKey::Y},
            {"KeyZ", KeyboardKey::Z},
            {"OSLeft", KeyboardKey::LEFT_SUPER},
            {"OSRight", KeyboardKey::RIGHT_SUPER},
            {"Delete", KeyboardKey::DEL},
            {"NumpadEnter", KeyboardKey::SEPARATOR},
            {"NumpadDigit0", KeyboardKey::NUMPAD_0},
            {"NumpadDigit1", KeyboardKey::NUMPAD_1},
            {"NumpadDigit2", KeyboardKey::NUMPAD_2},
            {"NumpadDigit3", KeyboardKey::NUMPAD_3},
            {"NumpadDigit4", KeyboardKey::NUMPAD_4},
            {"NumpadDigit5", KeyboardKey::NUMPAD_5},
            {"NumpadDigit6", KeyboardKey::NUMPAD_6},
            {"NumpadDigit7", KeyboardKey::NUMPAD_7},
            {"NumpadDigit8", KeyboardKey::NUMPAD_8},
            {"NumpadDigit9", KeyboardKey::NUMPAD_9},
            {"NumpadMultiply", KeyboardKey::MULTIPLY},
            {"NumpadAdd", KeyboardKey::ADD},
            {"NumpadSubtract", KeyboardKey::SUBTRACT},
            {"NumpadDecimal", KeyboardKey::DECIMAL},
            {"NumpadDivide", KeyboardKey::DIVIDE},
            {"F1", KeyboardKey::F1},
            {"F2", KeyboardKey::F2},
            {"F3", KeyboardKey::F3},
            {"F4", KeyboardKey::F4},
            {"F5", KeyboardKey::F5},
            {"F6", KeyboardKey::F6},
            {"F7", KeyboardKey::F7},
            {"F8", KeyboardKey::F8},
            {"F9", KeyboardKey::F9},
            {"F10", KeyboardKey::F10},
            {"F11", KeyboardKey::F11},
            {"F12", KeyboardKey::F12},
            {"NumLock", KeyboardKey::NUMLOCK},
            {"ScrollLock", KeyboardKey::SCROLL},
            {"Semicolon", KeyboardKey::SEMICOLON},
            {"Equal", KeyboardKey::EQUAL},
            {"Comma", KeyboardKey::COMMA},
            {"Minus", KeyboardKey::MINUS},
            {"Period", KeyboardKey::PERIOD},
            {"Slash", KeyboardKey::SLASH},
            {"Backquote", KeyboardKey::GRAVE},
            {"BracketLeft", KeyboardKey::LEFT_BRACKET},
            {"Backslash", KeyboardKey::BACKSLASH},
            {"BracketRight", KeyboardKey::RIGHT_BRACKET},
            {"Quote", KeyboardKey::QUOTE},
            {"IntlBackslash", KeyboardKey::LESS}
        };

        InputManagerEm::InputManagerEm()
        {
            emscripten_set_keypress_callback(nullptr, this, true, emKeyCallback);
            emscripten_set_keydown_callback(nullptr, this, true, emKeyCallback);
            emscripten_set_keyup_callback(nullptr, this, true, emKeyCallback);

            emscripten_set_mousedown_callback("#canvas", this, true, emMouseCallback);
            emscripten_set_mouseup_callback("#canvas", this, true, emMouseCallback);
            emscripten_set_mousemove_callback("#canvas", this, true, emMouseCallback);
            emscripten_set_wheel_callback("#canvas", this, true, emWheelCallback);
            emscripten_set_pointerlockchange_callback(nullptr, this, true, emPointerLockChangeCallback);

            emscripten_set_gamepadconnected_callback(this, true, emGamepadCallback);
            emscripten_set_gamepaddisconnected_callback(this, true, emGamepadCallback);

            int result = emscripten_get_num_gamepads();

            if (result == EMSCRIPTEN_RESULT_NOT_SUPPORTED)
                Log(Log::Level::INFO) << "Gamepads not supported";
            else
            {
                for (long index = 0; index < result; ++index)
                    handleGamepadConnected(index);
            }
        }

        InputManagerEm::~InputManagerEm()
        {
        }

        void InputManagerEm::update()
        {
            for (const std::unique_ptr<Gamepad>& gamepad : gamepads)
            {
                GamepadEm* gamepadEm = static_cast<GamepadEm*>(gamepad.get());
                gamepadEm->update();
            }
        }

        KeyboardKey InputManagerEm::convertKeyCode(const EM_UTF8 key[32])
        {
            auto i = keyMap.find(key);

            if (i != keyMap.end())
                return i->second;
            else
                return KeyboardKey::NONE;
        }

        uint32_t InputManagerEm::getKeyboardModifiers(const EmscriptenKeyboardEvent* keyboardEvent)
        {
            uint32_t modifiers = 0;

            if (keyboardEvent->ctrlKey) modifiers |= ouzel::CONTROL_DOWN;
            if (keyboardEvent->shiftKey) modifiers |= ouzel::SHIFT_DOWN;
            if (keyboardEvent->altKey) modifiers |= ouzel::ALT_DOWN;
            if (keyboardEvent->metaKey) modifiers |= ouzel::SUPER_DOWN;

            return modifiers;
        }

        uint32_t InputManagerEm::getMouseModifiers(const EmscriptenMouseEvent* mouseEvent)
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

        void InputManagerEm::setCursorVisible(bool visible)
        {
            cursorVisible = visible;

            engine->executeOnMainThread([visible] {
                if (!visible)
                    emscripten_hide_mouse();
                else
                {
                    // TODO: replace this with emscripten_show_mouse when https://github.com/kripken/emscripten/issues/4614 is merged
                    emscripten_run_script("var styleSheet = document.styleSheets[0];"
                                          "var rules = styleSheet.cssRules;"
                                          "for (var i = 0; i < rules.length; i++) {"
                                          "if (rules[i].cssText.substr(0, 6) == 'canvas') {"
                                          "styleSheet.deleteRule(i);"
                                          "i--;"
                                          "}"
                                          "}"
                                          "styleSheet.insertRule('canvas.emscripten { border: none; cursor: auto; }', 0);");
                }
            });
        }

        bool InputManagerEm::isCursorVisible() const
        {
            return cursorVisible;
        }

        void InputManagerEm::setCursorLocked(bool locked)
        {
            engine->executeOnMainThread([locked] {
                if (locked)
                    emscripten_request_pointerlock(nullptr, false);
                else
                    emscripten_exit_pointerlock();
            });
        }

        bool InputManagerEm::isCursorLocked() const
        {
            return cursorLocked;
        }

        void InputManagerEm::pointerLockChanged(bool locked)
        {
            cursorLocked = locked;
        }

        void InputManagerEm::handleGamepadConnected(long index)
        {
            Event event;
            event.type = Event::Type::GAMEPAD_CONNECT;

            std::unique_ptr<GamepadEm> gamepad(new GamepadEm(index));

            event.gamepadEvent.gamepad = gamepad.get();

            gamepads.push_back(std::move(gamepad));

            engine->getEventDispatcher().postEvent(event);
        }

        void InputManagerEm::handleGamepadDisconnected(long index)
        {
            auto i = std::find_if(gamepads.begin(), gamepads.end(), [index](const std::unique_ptr<Gamepad>& gamepad) {
                GamepadEm* currentGamepad = static_cast<GamepadEm*>(gamepad.get());
                return currentGamepad->getIndex() == index;
            });

            if (i != gamepads.end())
            {
                Event event;
                event.type = Event::Type::GAMEPAD_DISCONNECT;

                event.gamepadEvent.gamepad = (*i).get();

                engine->getEventDispatcher().postEvent(event);

                gamepads.erase(i);
            }
        }
    } // namespace input
} // namespace ouzel
