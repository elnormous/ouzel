// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <array>
#include "input/Gamepad.hpp"

namespace ouzel
{
    namespace input
    {
        struct GamepadConfig
        {
            std::array<Gamepad::Button, 24> buttonMap;
            std::array<Gamepad::Axis, 6> axisMap; // X, Y, Z, RX, RY, RZ
        };

        // Playstation 3 controller
        // vendorId = 0x054C, productId = 0x0268
        static const GamepadConfig PLAYSTATION_3_CONFIG = {
            {
                Gamepad::Button::BACK, // Select
                Gamepad::Button::LEFT_THUMB, // L3
                Gamepad::Button::RIGHT_THUMB, // R3
                Gamepad::Button::START, // Start
                Gamepad::Button::DPAD_UP,
                Gamepad::Button::DPAD_RIGHT,
                Gamepad::Button::DPAD_DOWN,
                Gamepad::Button::DPAD_LEFT,
                Gamepad::Button::LEFT_TRIGGER, // L2
                Gamepad::Button::RIGHT_TRIGGER, // R2
                Gamepad::Button::LEFT_SHOULDER, // L1
                Gamepad::Button::RIGHT_SHOULDER, // R1
                Gamepad::Button::FACE_TOP, // Triangle
                Gamepad::Button::FACE_RIGHT, // Circle
                Gamepad::Button::FACE_BOTTOM, // Cross
                Gamepad::Button::FACE_LEFT, // Square
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE
            },
            {
                Gamepad::Axis::LEFT_THUMB_X,
                Gamepad::Axis::LEFT_THUMB_Y,
                Gamepad::Axis::RIGHT_THUMB_X,
                Gamepad::Axis::LEFT_TRIGGER,
                Gamepad::Axis::RIGHT_TRIGGER,
                Gamepad::Axis::RIGHT_THUMB_Y
            }
        };

        // Playstation 4 controller
        // vendorId = 0x054C, productId = 0x05C4
        static const GamepadConfig PLAYSTATION_4_CONFIG = {
            {
                Gamepad::Button::FACE_LEFT, // Square
                Gamepad::Button::FACE_BOTTOM, // Cross
                Gamepad::Button::FACE_RIGHT, // Circle
                Gamepad::Button::FACE_TOP, // Triangle
                Gamepad::Button::LEFT_SHOULDER, // L1
                Gamepad::Button::RIGHT_SHOULDER, // R1
                Gamepad::Button::LEFT_TRIGGER, // L2
                Gamepad::Button::RIGHT_TRIGGER, // R2
                Gamepad::Button::BACK, // Share
                Gamepad::Button::START, // Options
                Gamepad::Button::LEFT_THUMB, // L3
                Gamepad::Button::RIGHT_THUMB, // R3
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE
            },
            {
                Gamepad::Axis::LEFT_THUMB_X,
                Gamepad::Axis::LEFT_THUMB_Y,
                Gamepad::Axis::RIGHT_THUMB_X,
                Gamepad::Axis::LEFT_TRIGGER,
                Gamepad::Axis::RIGHT_TRIGGER,
                Gamepad::Axis::RIGHT_THUMB_Y
            }
        };

        // Xbox One controller
        // vendorId = 0x045E, productId = 0x02D1
        static const GamepadConfig XBOX_ONE_CONFIG = {
            {
                Gamepad::Button::FACE_BOTTOM, // A
                Gamepad::Button::FACE_RIGHT, // B
                Gamepad::Button::FACE_LEFT, // X
                Gamepad::Button::FACE_TOP, // Y
                Gamepad::Button::LEFT_SHOULDER,
                Gamepad::Button::RIGHT_SHOULDER,
                Gamepad::Button::LEFT_THUMB,
                Gamepad::Button::RIGHT_THUMB,
                Gamepad::Button::BACK, // Menu
                Gamepad::Button::START, // View
                Gamepad::Button::DPAD_UP,
                Gamepad::Button::DPAD_DOWN,
                Gamepad::Button::DPAD_LEFT,
                Gamepad::Button::DPAD_RIGHT,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE
            },
            {
                Gamepad::Axis::LEFT_THUMB_X,
                Gamepad::Axis::LEFT_THUMB_Y,
                Gamepad::Axis::RIGHT_THUMB_X,
                Gamepad::Axis::RIGHT_THUMB_Y,
                Gamepad::Axis::LEFT_TRIGGER,
                Gamepad::Axis::RIGHT_TRIGGER
            }
        };

        // Xbox 360 controller
        static const GamepadConfig XBOX_360_CONFIG = {
            {
                Gamepad::Button::FACE_BOTTOM, // A
                Gamepad::Button::FACE_RIGHT, // B
                Gamepad::Button::FACE_LEFT, // X
                Gamepad::Button::FACE_TOP, // Y
                Gamepad::Button::LEFT_SHOULDER,
                Gamepad::Button::RIGHT_SHOULDER,
                Gamepad::Button::LEFT_THUMB,
                Gamepad::Button::RIGHT_THUMB,
                Gamepad::Button::START,
                Gamepad::Button::BACK,
                Gamepad::Button::DPAD_UP,
                Gamepad::Button::DPAD_DOWN,
                Gamepad::Button::DPAD_LEFT,
                Gamepad::Button::DPAD_RIGHT,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE
            },
            {
                Gamepad::Axis::LEFT_THUMB_X,
                Gamepad::Axis::LEFT_THUMB_Y,
                Gamepad::Axis::LEFT_TRIGGER,
                Gamepad::Axis::RIGHT_THUMB_X,
                Gamepad::Axis::RIGHT_THUMB_Y,
                Gamepad::Axis::RIGHT_TRIGGER
            }
        };

        // Acme GA07 controller
        // vendorId = 0x0079, productId = 0x0006
        static const GamepadConfig ACME_GA07_CONFIG = {
            {
                Gamepad::Button::FACE_TOP, // 1
                Gamepad::Button::FACE_RIGHT, // 2
                Gamepad::Button::FACE_BOTTOM, // 3
                Gamepad::Button::FACE_LEFT, // 4
                Gamepad::Button::LEFT_SHOULDER,
                Gamepad::Button::RIGHT_SHOULDER,
                Gamepad::Button::LEFT_TRIGGER,
                Gamepad::Button::RIGHT_TRIGGER,
                Gamepad::Button::BACK,
                Gamepad::Button::START,
                Gamepad::Button::LEFT_THUMB,
                Gamepad::Button::RIGHT_THUMB,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE
            },
            {
                Gamepad::Axis::LEFT_THUMB_X,
                Gamepad::Axis::LEFT_THUMB_Y,
                Gamepad::Axis::RIGHT_THUMB_X,
                Gamepad::Axis::NONE,
                Gamepad::Axis::NONE,
                Gamepad::Axis::RIGHT_THUMB_Y
            }
        };

        // Generic controller
        static const GamepadConfig GENERIC_CONFIG = {
            {
                Gamepad::Button::FACE_LEFT,
                Gamepad::Button::FACE_BOTTOM,
                Gamepad::Button::FACE_RIGHT,
                Gamepad::Button::FACE_TOP,
                Gamepad::Button::LEFT_SHOULDER,
                Gamepad::Button::RIGHT_SHOULDER,
                Gamepad::Button::LEFT_TRIGGER,
                Gamepad::Button::RIGHT_TRIGGER,
                Gamepad::Button::BACK,
                Gamepad::Button::START,
                Gamepad::Button::LEFT_THUMB,
                Gamepad::Button::RIGHT_THUMB,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE
            },
            {
                Gamepad::Axis::LEFT_THUMB_X,
                Gamepad::Axis::LEFT_THUMB_Y,
                Gamepad::Axis::RIGHT_THUMB_X,
                Gamepad::Axis::LEFT_TRIGGER,
                Gamepad::Axis::RIGHT_TRIGGER,
                Gamepad::Axis::RIGHT_THUMB_Y
            }
        };
    }
}
