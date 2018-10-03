// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <emscripten.h>
#include "InputManagerEm.hpp"
#include "InputSystemEm.hpp"
#include "GamepadDeviceEm.hpp"
#include "core/Engine.hpp"
#include "core/Window.hpp"
#include "events/Event.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace input
    {
        InputManagerEm::InputManagerEm()
        {
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
                GamepadDeviceEm* GamepadDeviceEm = static_cast<GamepadDeviceEm*>(gamepad.get());
                GamepadDeviceEm->update();
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
    } // namespace input
} // namespace ouzel
