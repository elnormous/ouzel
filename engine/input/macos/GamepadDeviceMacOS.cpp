// Ouzel by Elviss Strazdins

#include "GamepadDeviceMacOS.hpp"
#include "InputSystemMacOS.hpp"

namespace ouzel::input::macos
{
    GamepadDevice::GamepadDevice(InputSystem& initInputSystem, DeviceId initId):
        input::GamepadDevice{initInputSystem, initId}
    {
    }
}
