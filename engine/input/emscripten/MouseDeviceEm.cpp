// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <emscripten.h>
#include <emscripten/html5.h>
#include "MouseDeviceEm.hpp"
#include "input/InputSystem.hpp"

namespace ouzel
{
    namespace input
    {
        void MouseDeviceEm::setCursorVisible(bool visible)
        {
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
        }

        void MouseDeviceEm::setCursorLocked(bool locked)
        {
            if (locked)
                emscripten_request_pointerlock(nullptr, false);
            else
                emscripten_exit_pointerlock();
        }
    } // namespace input
} // namespace ouzel
