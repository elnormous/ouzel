// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <stdexcept>
#include <emscripten.h>
#include <emscripten/html5.h>
#include "GamepadDeviceEm.hpp"

namespace ouzel
{
    namespace input
    {
        static constexpr float THUMB_DEADZONE = 0.2F;

        // based on https://w3c.github.io/gamepad/#remapping
        static Gamepad::Button buttonMap[17] = {
            Gamepad::Button::FaceBottom, // 0
            Gamepad::Button::FaceRight, // 1
            Gamepad::Button::FaceLeft, // 2
            Gamepad::Button::FaceTop, // 3
            Gamepad::Button::LeftShoulder, // 4
            Gamepad::Button::RightShoulder, // 5
            Gamepad::Button::LeftTrigger, // 6
            Gamepad::Button::RightTrigger, // 7
            Gamepad::Button::Back, // 8
            Gamepad::Button::Start, // 9
            Gamepad::Button::LeftThumb, // 10
            Gamepad::Button::RightThumb, // 11
            Gamepad::Button::DpadUp, // 12
            Gamepad::Button::DpadDown, // 13
            Gamepad::Button::DpadLeft, // 14
            Gamepad::Button::DpadRight, // 15
            Gamepad::Button::Pause // 16
        };

        GamepadDeviceEm::GamepadDeviceEm(InputSystem& initInputSystem,
                                         uint32_t initId,
                                         long initIndex):
            GamepadDevice(initInputSystem, initId),
            index(initIndex)
        {
        }

        void GamepadDeviceEm::update()
        {
            EmscriptenGamepadEvent event;

            if (emscripten_get_gamepad_status(index, &event) != EMSCRIPTEN_RESULT_SUCCESS)
                throw std::runtime_error("Failed to get gamepad status");

            for (int i = 0; i < event.numAxes && i <= 3; ++i)
            {
                if (event.axis[i] != axis[i])
                {
                    switch (i)
                    {
                        case 0:
                            handleThumbAxisChange(event.axis[i], axis[i], Gamepad::Button::LeftThumbLeft, Gamepad::Button::LeftThumbRight);
                            break;
                        case 1:
                            handleThumbAxisChange(event.axis[i], axis[i], Gamepad::Button::LeftThumbUp, Gamepad::Button::LeftThumbDown);
                            break;
                        case 2:
                            handleThumbAxisChange(event.axis[i], axis[i], Gamepad::Button::RightThumbLeft, Gamepad::Button::RightThumbRight);
                            break;
                        case 3:
                            handleThumbAxisChange(event.axis[i], axis[i], Gamepad::Button::RightThumbUp, Gamepad::Button::RightThumbDown);
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

        void GamepadDeviceEm::handleThumbAxisChange(double oldValue, double newValue,
                                              Gamepad::Button negativeButton, Gamepad::Button positiveButton)
        {
            if (newValue > 0.0)
            {
                handleButtonValueChange(positiveButton,
                                        static_cast<float>(newValue) > THUMB_DEADZONE,
                                        static_cast<float>(newValue));
            }
            else if (newValue < 0.0)
            {
                handleButtonValueChange(negativeButton,
                                        -static_cast<float>(newValue) > THUMB_DEADZONE,
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
    } // namespace input
} // namespace ouzel
