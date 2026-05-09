#include "GamepadDeviceMacOS.hpp"
#include "InputSystemMacOS.hpp"

namespace fluxion::input::macos
{
    GamepadDevice::GamepadDevice(InputSystem& initInputSystem, DeviceId initId):
        input::GamepadDevice{initInputSystem, initId}
    {
    }
}
