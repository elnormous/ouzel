// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include "math/Vector2.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystem;
        class KeyboardDevice;
        class GamepadDevice;
        class MouseDevice;
        class TouchpadDevice;

        class EventDevice final
        {
        public:
            EventDevice(InputSystem& inputSystem, const std::string& initFilename);
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
        };
    } // namespace input
} // namespace ouzel
