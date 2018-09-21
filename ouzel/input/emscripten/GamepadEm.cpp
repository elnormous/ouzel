// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include <map>
#include <emscripten.h>
#include <emscripten/html5.h>
#include "GamepadEm.hpp"
#include "utils/Errors.hpp"

static const float THUMB_DEADZONE = 0.2F;

namespace ouzel
{
    namespace input
    {
        // based on https://w3c.github.io/gamepad/#remapping
        static Gamepad::Button buttonMap[17] = {
            Gamepad::Button::FACE_BOTTOM, // 0
            Gamepad::Button::FACE_RIGHT, // 1
            Gamepad::Button::FACE_LEFT, // 2
            Gamepad::Button::FACE_TOP, // 3
            Gamepad::Button::LEFT_SHOULDER, // 4
            Gamepad::Button::RIGHT_SHOULDER, // 5
            Gamepad::Button::LEFT_TRIGGER, // 6
            Gamepad::Button::RIGHT_TRIGGER, // 7
            Gamepad::Button::BACK, // 8
            Gamepad::Button::START, // 9
            Gamepad::Button::LEFT_THUMB, // 10
            Gamepad::Button::RIGHT_THUMB, // 11
            Gamepad::Button::DPAD_UP, // 12
            Gamepad::Button::DPAD_DOWN, // 13
            Gamepad::Button::DPAD_LEFT, // 14
            Gamepad::Button::DPAD_RIGHT, // 15
            Gamepad::Button::PAUSE // 16
        };

        GamepadEm::GamepadEm(long initIndex):
            index(initIndex)
        {
            std::fill(std::begin(axis), std::end(axis), 0.0F);
            std::fill(std::begin(analogButton), std::end(analogButton), 0.0F);
        }

        void GamepadEm::update()
        {
            EmscriptenGamepadEvent event;

            if (emscripten_get_gamepad_status(index, &event) != EMSCRIPTEN_RESULT_SUCCESS)
                throw SystemError("Failed to get gamepad status");

            for (int i = 0; i < event.numAxes && i <= 3; ++i)
            {
                if (event.axis[i] != axis[i])
                {
                    if (i == 0) handleThumbAxisChange(event.axis[i], axis[i], Gamepad::Button::LEFT_THUMB_LEFT, Gamepad::Button::LEFT_THUMB_RIGHT);
                    else if (i == 1) handleThumbAxisChange(event.axis[i], axis[i], Gamepad::Button::LEFT_THUMB_UP, Gamepad::Button::LEFT_THUMB_DOWN);
                    else if (i == 2) handleThumbAxisChange(event.axis[i], axis[i], Gamepad::Button::RIGHT_THUMB_LEFT, Gamepad::Button::RIGHT_THUMB_RIGHT);
                    else if (i == 3) handleThumbAxisChange(event.axis[i], axis[i], Gamepad::Button::RIGHT_THUMB_UP, Gamepad::Button::RIGHT_THUMB_DOWN);

                    axis[i] = event.axis[i];
                }
            }

            for (int i = 0; i < event.numButtons && i <= 16; ++i)
            {
                if (event.analogButton[i] != analogButton[i])
                {
                    handleButtonValueChange(buttonMap[i], event.digitalButton[i], static_cast<float>(analogButton[i]));
                    analogButton[i] = event.analogButton[i];
                }
            }
        }

        void GamepadEm::handleThumbAxisChange(double oldValue, double newValue,
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
