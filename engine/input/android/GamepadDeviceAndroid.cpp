// Ouzel by Elviss Strazdins

#include "GamepadDeviceAndroid.hpp"
#include "InputSystemAndroid.hpp"

namespace ouzel::input::android
{
    GamepadDevice::GamepadDevice(InputSystem& initInputSystem,
                                 DeviceId initId):
        input::GamepadDevice(initInputSystem, initId)
    {
    }
}
