// Ouzel by Elviss Strazdins

#include "GamepadDeviceWin.hpp"
#include "InputSystemWin.hpp"

namespace ouzel::input::windows
{
    GamepadDevice::GamepadDevice(InputSystem& initInputSystem,
                                 DeviceId initId):
        input::GamepadDevice(initInputSystem, initId)
    {
    }
}
