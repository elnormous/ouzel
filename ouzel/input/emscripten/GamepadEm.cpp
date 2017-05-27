// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include <emscripten.h>
#include "GamepadEm.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace input
    {
        GamepadEm::GamepadEm(long aIndex):
            index(aIndex)
        {
            std::fill(std::begin(axis), std::end(axis), 0.0f);
            std::fill(std::begin(analogButton), std::end(analogButton), 0.0f);
            std::fill(std::begin(digitalButton), std::end(digitalButton), false);
        }

        void GamepadEm::update()
        {
            EmscriptenGamepadEvent event;

            if (emscripten_get_gamepad_status(index, &event) != EMSCRIPTEN_RESULT_SUCCESS)
            {
                Log(Log::Level::ERR) << "Failed to get gamepad status";
                return;
            }

            for (int i = 0; i < event.numAxes; ++i)
            {
                if (event.axis[i] != axis[i])
                {
                    event.axis[i] = axis[i];
                    Log() << "axis " << i << " changed to " << event.axis[i];
                    // TODO: dispatch event
                }
            }

            for (int i = 0; i < event.numButtons; ++i)
            {
                if (event.analogButton[i] != analogButton[i])
                {
                    event.analogButton[i] = analogButton[i];
                    Log() << "analogButton " << i << " changed to " << event.analogButton[i];
                    // TODO: dispatch event
                }
            }

            for (int i = 0; i < 64; ++i)
            {
                if (event.digitalButton[i] != digitalButton[i])
                {
                    event.digitalButton[i] = digitalButton[i];
                    Log() << "digitalButton " << i << " changed to " << event.digitalButton[i];
                    // TODO: dispatch event
                }
            }
        }
    } // namespace input
} // namespace ouzel
