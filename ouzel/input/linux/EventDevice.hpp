// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include "math/Vector2.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystemLinux;
        class KeyboardDevice;
        class GamepadDevice;
        class MouseDevice;
        class TouchpadDevice;

        class EventDevice final
        {
        public:
            EventDevice(InputSystemLinux& inputSystem, const std::string& initFilename);
            ~EventDevice();

            EventDevice(const EventDevice& other) = delete;
            EventDevice& operator=(const EventDevice& other) = delete;

            EventDevice(EventDevice&& other)
            {
                fd = other.fd;
                name = std::move(name);
                vendor = other.vendor;
                product = other.product;
                keyboardDevice = std::move(other.keyboardDevice);
                gamepadDevice = std::move(other.gamepadDevice);
                mouseDevice = std::move(other.mouseDevice);
                touchpadDevice = std::move(other.touchpadDevice);

                other.fd = -1;
                other.vendor = 0;
                other.product = 0;
            }

            EventDevice& operator=(EventDevice&& other)
            {
                if (&other != this)
                {
                    fd = other.fd;
                    name = std::move(name);
                    vendor = other.vendor;
                    product = other.product;
                    keyboardDevice = std::move(other.keyboardDevice);
                    gamepadDevice = std::move(other.gamepadDevice);
                    mouseDevice = std::move(other.mouseDevice);
                    touchpadDevice = std::move(other.touchpadDevice);

                    other.fd = -1;
                    other.vendor = 0;
                    other.product = 0;
                }

                return *this;
            }

            void update();

            inline int getFd() const { return fd; }

        private:
            int fd = -1;
            std::string filename;
            std::string name;
            uint16_t vendor = 0;
            uint16_t product = 0;

            std::unique_ptr<KeyboardDevice> keyboardDevice;
            std::unique_ptr<GamepadDevice> gamepadDevice;
            std::unique_ptr<MouseDevice> mouseDevice;
            std::unique_ptr<TouchpadDevice> touchpadDevice;

            Vector2 cursorPosition;
            struct Slot
            {
                int32_t trackingId;
                int32_t positionX;
                int32_t positionY;
                int32_t pressure = 1;
                enum class Action
                {
                    NONE,
                    BEGIN,
                    END,
                    MOVE
                };
                Action action = Action::NONE;
            };
            std::vector<Slot> touchSlots;
            int32_t currentTouchSlot = 0;
            int32_t touchMinX;
            int32_t touchMaxX;
            int32_t touchMinY;
            int32_t touchMaxY;
            int32_t touchRangeX;
            int32_t touchRangeY;
            int32_t touchMinPressure = 0;
            int32_t touchMaxPressure = 1;

            int32_t hat0XValue = 0;
            int32_t hat0YValue = 0;
        };
    } // namespace input
} // namespace ouzel
