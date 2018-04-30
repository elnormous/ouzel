// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstdint>
#include <string>

namespace ouzel
{
    namespace input
    {
        class EventDevice
        {
        public:
            enum DeviceClass
            {
                CLASS_NONE = 0,
                CLASS_KEYBOARD = 1,
                CLASS_MOUSE = 2,
                CLASS_TOUCHPAD = 4,
                CLASS_GAMEPAD = 8
            };

            EventDevice(const std::string& filename);
            ~EventDevice();

            EventDevice(const EventDevice& other) = delete;
            EventDevice& operator=(const EventDevice& other) = delete;

            EventDevice(EventDevice&& other)
            {
                fd = other.fd;
                deviceClass = other.deviceClass;
                other.fd = -1;
                other.deviceClass = 0;
            }

            EventDevice& operator=(EventDevice&& other)
            {
                if (&other != this)
                {
                    fd = other.fd;
                    deviceClass = other.deviceClass;
                    other.fd = -1;
                    other.deviceClass = 0;
                }

                return *this;
            }

            inline uint32_t getDeviceClass() const { return deviceClass; }
            inline int getFd() const { return fd; }

        private:
            uint32_t deviceClass = CLASS_NONE;
            int fd = -1;
        };
    } // namespace input
} // namespace ouzel
