// Ouzel by Elviss Strazdins

#include <stdexcept>
#include <emscripten.h>
#include <emscripten/html5.h>
#include "GamepadDeviceEm.hpp"
#include "InputSystemEm.hpp"

namespace ouzel::input::emscripten
{
    namespace
    {
        constexpr float thumbDeadzone = 0.2F;

        // based on https://w3c.github.io/gamepad/#remapping
        static Gamepad::Button buttonMap[17] = {
            Gamepad::Button::faceBottom, // 0
            Gamepad::Button::faceRight, // 1
            Gamepad::Button::faceLeft, // 2
            Gamepad::Button::faceTop, // 3
            Gamepad::Button::leftShoulder, // 4
            Gamepad::Button::rightShoulder, // 5
            Gamepad::Button::leftTrigger, // 6
            Gamepad::Button::rightTrigger, // 7
            Gamepad::Button::back, // 8
            Gamepad::Button::start, // 9
            Gamepad::Button::leftThumb, // 10
            Gamepad::Button::rightThumb, // 11
            Gamepad::Button::dPadUp, // 12
            Gamepad::Button::dPadDown, // 13
            Gamepad::Button::dPadLeft, // 14
            Gamepad::Button::dPadRight, // 15
            Gamepad::Button::pause // 16
        };
    }

    GamepadDevice::GamepadDevice(InputSystem& initInputSystem,
                                 DeviceId initId,
                                 long initIndex):
        input::GamepadDevice{initInputSystem, initId},
        index{initIndex}
    {
    }

    void GamepadDevice::update()
    {
        EmscriptenGamepadEvent event;

        if (emscripten_get_gamepad_status(index, &event) != EMSCRIPTEN_RESULT_SUCCESS)
            throw std::runtime_error{"Failed to get gamepad status"};

        for (int i = 0; i < event.numAxes && i <= 3; ++i)
        {
            if (event.axis[i] != axis[i])
            {
                switch (i)
                {
                    case 0:
                        handleThumbAxisChange(event.axis[i], axis[i], Gamepad::Button::leftThumbLeft, Gamepad::Button::leftThumbRight);
                        break;
                    case 1:
                        handleThumbAxisChange(event.axis[i], axis[i], Gamepad::Button::leftThumbUp, Gamepad::Button::leftThumbDown);
                        break;
                    case 2:
                        handleThumbAxisChange(event.axis[i], axis[i], Gamepad::Button::rightThumbLeft, Gamepad::Button::rightThumbRight);
                        break;
                    case 3:
                        handleThumbAxisChange(event.axis[i], axis[i], Gamepad::Button::rightThumbUp, Gamepad::Button::rightThumbDown);
                        break;
                }

                axis[i] = event.axis[i];
            }
        }

        for (int i = 0; i < event.numButtons && i <= 16; ++i)
        {
            if (event.analogButton[i] != analogButton[i])
            {
                handleButtonValueChange(buttonMap[i],
                                        event.digitalButton[i],
                                        static_cast<float>(analogButton[i]));
                analogButton[i] = event.analogButton[i];
            }
        }
    }

    void GamepadDevice::handleThumbAxisChange(double oldValue, double newValue,
                                              Gamepad::Button negativeButton, Gamepad::Button positiveButton)
    {
        if (newValue > 0.0)
        {
            handleButtonValueChange(positiveButton,
                                    static_cast<float>(newValue) > thumbDeadzone,
                                    static_cast<float>(newValue));
        }
        else if (newValue < 0.0)
        {
            handleButtonValueChange(negativeButton,
                                    -static_cast<float>(newValue) > thumbDeadzone,
                                    -static_cast<float>(newValue));
        }
        else // thumbstick is 0
        {
            if (oldValue > newValue)
                handleButtonValueChange(positiveButton, false, 0.0F);
            else
                handleButtonValueChange(negativeButton, false, 0.0F);
        }
    }
}
