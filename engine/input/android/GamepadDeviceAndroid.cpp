#include "GamepadDeviceAndroid.hpp"
#include "InputSystemAndroid.hpp"

namespace fluxion::input::android
{
    GamepadDevice::GamepadDevice(InputSystem& initInputSystem,
                                 DeviceId initId):
        input::GamepadDevice{initInputSystem, initId}
    {
    }
}
