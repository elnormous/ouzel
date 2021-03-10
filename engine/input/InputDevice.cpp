// Ouzel by Elviss Strazdins

#include "InputDevice.hpp"
#include "InputSystem.hpp"

namespace ouzel::input
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
