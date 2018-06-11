// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include <map>
#include <emscripten.h>
#include <emscripten/html5.h>
#include "GamepadEm.hpp"
#include "utils/Log.hpp"

static const float THUMB_DEADZONE = 0.2F;

namespace ouzel
{
    namespace input
    {
        // based on https://w3c.github.io/gamepad/#remapping
        static GamepadButton buttonMap[17] = {
            GamepadButton::FACE_BOTTOM, // 0
            GamepadButton::FACE_RIGHT, // 1
            GamepadButton::FACE_LEFT, // 2
            GamepadButton::FACE_TOP, // 3
            GamepadButton::LEFT_SHOULDER, // 4
            GamepadButton::RIGHT_SHOULDER, // 5
            GamepadButton::LEFT_TRIGGER, // 6
            GamepadButton::RIGHT_TRIGGER, // 7
            GamepadButton::BACK, // 8
            GamepadButton::START, // 9
            GamepadButton::LEFT_THUMB, // 10
            GamepadButton::RIGHT_THUMB, // 11
            GamepadButton::DPAD_UP, // 12
            GamepadButton::DPAD_DOWN, // 13
            GamepadButton::DPAD_LEFT, // 14
            GamepadButton::DPAD_RIGHT, // 15
            GamepadButton::PAUSE // 16
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
            {
                Log(Log::Level::ERR) << "Failed to get gamepad status";
                return;
            }

            for (int i = 0; i < event.numAxes && i <= 3; ++i)
            {
                if (event.axis[i] != axis[i])
                {
                    if (i == 0) handleThumbAxisChange(event.axis[i], axis[i], GamepadButton::LEFT_THUMB_LEFT, GamepadButton::LEFT_THUMB_RIGHT);
                    else if (i == 1) handleThumbAxisChange(event.axis[i], axis[i], GamepadButton::LEFT_THUMB_UP, GamepadButton::LEFT_THUMB_DOWN);
                    else if (i == 2) handleThumbAxisChange(event.axis[i], axis[i], GamepadButton::RIGHT_THUMB_LEFT, GamepadButton::RIGHT_THUMB_RIGHT);
                    else if (i == 3) handleThumbAxisChange(event.axis[i], axis[i], GamepadButton::RIGHT_THUMB_UP, GamepadButton::RIGHT_THUMB_DOWN);

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
                                              GamepadButton negativeButton, GamepadButton positiveButton)
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
