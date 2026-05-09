#include "InputDevice.hpp"
#include "InputSystem.hpp"

namespace fluxion::input
{
    InputDevice::InputDevice(InputSystem& initInputSystem, DeviceId initId, Controller::Type initType):
        inputSystem{initInputSystem}, id{initId}, type{initType}
    {
        inputSystem.addInputDevice(*this);
    }

    InputDevice::~InputDevice()
    {
        inputSystem.removeInputDevice(*this);
    }
}
