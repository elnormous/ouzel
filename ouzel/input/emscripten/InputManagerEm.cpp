// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <emscripten.h>
#include "InputManagerEm.hpp"
#include "InputSystemEm.hpp"
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
            inputEm->keyPress(ouzel::input::InputSystemEm::convertKeyCode(keyEvent->code),
                              ouzel::input::InputSystemEm::getKeyboardModifiers(keyEvent));
            return true;
        case EMSCRIPTEN_EVENT_KEYUP:
            inputEm->keyRelease(ouzel::input::InputSystemEm::convertKeyCode(keyEvent->code),
                                ouzel::input::InputSystemEm::getKeyboardModifiers(keyEvent));
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
                                      ouzel::input::InputSystemEm::getMouseModifiers(mouseEvent));
            return true;
        case EMSCRIPTEN_EVENT_MOUSEUP:
            inputEm->mouseButtonRelease(button,
                                        ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position),
                                        ouzel::input::InputSystemEm::getMouseModifiers(mouseEvent));
            return true;
        case EMSCRIPTEN_EVENT_MOUSEMOVE:
            inputEm->mouseMove(ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position),
                               ouzel::input::InputSystemEm::getMouseModifiers(mouseEvent));
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
                             ouzel::input::InputSystemEm::getMouseModifiers(&wheelEvent->mouse));

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

static EM_BOOL emTouchCallback(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData)
{
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
}

namespace ouzel
{
    namespace input
    {
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

            emscripten_set_touchstart_callback("#canvas", this, true, emTouchCallback);
            emscripten_set_touchend_callback("#canvas", this, true, emTouchCallback);
            emscripten_set_touchmove_callback("#canvas", this, true, emTouchCallback);
            emscripten_set_touchcancel_callback("#canvas", this, true, emTouchCallback);

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
            /*for (const std::unique_ptr<Gamepad>& gamepad : gamepads)
            {
                GamepadEm* gamepadEm = static_cast<GamepadEm*>(gamepad.get());
                gamepadEm->update();
            }*/
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

            /*std::unique_ptr<GamepadEm> gamepad(new GamepadEm(index));
            event.gamepadEvent.gamepad = gamepad.get();
            gamepads.push_back(std::move(gamepad));*/

            engine->getEventDispatcher().postEvent(event);
        }

        void InputManagerEm::handleGamepadDisconnected(long index)
        {
            /*auto i = std::find_if(gamepads.begin(), gamepads.end(), [index](const std::unique_ptr<Gamepad>& gamepad) {
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
            }*/
        }
    } // namespace input
} // namespace ouzel
