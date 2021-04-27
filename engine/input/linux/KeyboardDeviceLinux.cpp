// Ouzel by Elviss Strazdins

#include "../../core/Setup.h"
#include "KeyboardDeviceLinux.hpp"
#include "InputSystemLinux.hpp"

namespace ouzel::input::linux
{
    KeyboardDevice::KeyboardDevice(InputSystem& initInputSystem,
                                   DeviceId initId):
        input::KeyboardDevice{initInputSystem, initId}
    {
    }
}