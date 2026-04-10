#include "GamepadDeviceWin.hpp"
#include "InputSystemWin.hpp"

namespace fluxion::input::windows
{
    GamepadDevice::GamepadDevice(InputSystem& initInputSystem,
                                 DeviceId initId):
        input::GamepadDevice{initInputSystem, initId}
    {
    }
}
