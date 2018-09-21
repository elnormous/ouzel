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

        Keyboard::Key InputManagerEm::convertKeyCode(const EM_UTF8 key[32])
        {
            auto i = keyMap.find(key);

            if (i != keyMap.end())
                return i->second;
            else
                return Keyboard::Key::NONE;
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
