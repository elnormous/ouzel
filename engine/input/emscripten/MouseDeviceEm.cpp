// Ouzel by Elviss Strazdins

#include <emscripten.h>
#include <emscripten/html5.h>
#include "MouseDeviceEm.hpp"
#include "InputSystemEm.hpp"

namespace ouzel::input::emscripten
{
    MouseDevice::MouseDevice(InputSystem& initInputSystem,
                             DeviceId initId):
        input::MouseDevice(initInputSystem, initId)
    {
    }

    void MouseDevice::setCursorVisible(bool visible)
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

    void MouseDevice::setCursorLocked(bool locked)
    {
        if (locked)
            emscripten_request_pointerlock(nullptr, false);
        else
            emscripten_exit_pointerlock();
    }
}
