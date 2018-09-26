// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

namespace ouzel
{
    namespace input
    {
        class Controller
        {
        public:
            enum Type
            {
                KEYBOARD,
                MOUSE,
                TOUCHPAD,
                GAMEPAD
            };

            explicit Controller(Type initType, uint32_t initDeviceId):
                type(initType), deviceId(initDeviceId)
            {}
            virtual ~Controller() {}

            inline Type getType() const { return type; }
            inline uint32_t getDeviceId() const { return deviceId; }

        private:
            Type type;
            uint32_t deviceId;
        };
    }
}
