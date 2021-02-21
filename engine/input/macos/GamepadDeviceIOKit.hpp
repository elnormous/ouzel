// Ouzel by Elviss Strazdins

#ifndef OUZEL_INPUT_GAMEPADDEVICEIOKIT_HPP
#define OUZEL_INPUT_GAMEPADDEVICEIOKIT_HPP

#include <unordered_map>
#include <IOKit/hid/IOHIDManager.h>

#include "GamepadDeviceMacOS.hpp"
#include "../Gamepad.hpp"

namespace ouzel::input::macos
{
    class GamepadDeviceIOKit final: public GamepadDevice
    {
    public:
        GamepadDeviceIOKit(InputSystem& initInputSystem,
                           DeviceId initId,
                           IOHIDDeviceRef initDevice);

        auto getDevice() const noexcept { return device; }

        void handleInput(IOHIDValueRef value);

    private:
        void handleAxisChange(CFIndex oldValue, CFIndex newValue,
                              CFIndex min, CFIndex range,
                              Gamepad::Button negativeButton,
                              Gamepad::Button positiveButton);

        IOHIDDeviceRef device = nullptr;

        IOHIDElementRef hatElement = nullptr;
        CFIndex hatValue = 8;

        struct Button final
        {
            Gamepad::Button button = Gamepad::Button::none;
            CFIndex value = 0;
        };

        std::unordered_map<IOHIDElementRef, Button> buttons;

        struct Axis final
        {
            Gamepad::Axis axis = Gamepad::Axis::none;
            CFIndex min = 0;
            CFIndex max = 0;
            CFIndex range = 0;
            CFIndex value = 0;
            Gamepad::Button negativeButton = Gamepad::Button::none;
            Gamepad::Button positiveButton = Gamepad::Button::none;
        };

        std::unordered_map<IOHIDElementRef, Axis> axes;

        bool hasLeftTrigger = false;
        bool hasRightTrigger = false;
    };
}

#endif // OUZEL_INPUT_GAMEPADDEVICEIOKIT_HPP
