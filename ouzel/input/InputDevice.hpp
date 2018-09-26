// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

namespace ouzel
{
    namespace input
    {
        class InputDevice
        {
        public:
            enum Type
            {
                KEYBOARD,
                MOUSE,
                TOUCHPAD,
                GAMEPAD
            };

            explicit InputDevice(Type initType, uint32_t initDeviceId):
                type(initType), deviceId(initDeviceId)
            {}
            virtual ~InputDevice() {}

            inline Type getType() const { return type; }
            inline uint32_t getDeviceId() const { return deviceId; }

        private:
            Type type;
            uint32_t deviceId;
        };
    }
}
