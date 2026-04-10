#include "../../core/Setup.h"
#include "KeyboardDeviceLinux.hpp"
#include "InputSystemLinux.hpp"

namespace fluxion::input::linux
{
    KeyboardDevice::KeyboardDevice(InputSystem& initInputSystem,
                                   DeviceId initId):
        input::KeyboardDevice{initInputSystem, initId}
    {
    }
}