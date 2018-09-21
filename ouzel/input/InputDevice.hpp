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

            explicit InputDevice(Type initType): type(initType) {}
            virtual ~InputDevice() {}

            Type getType() const { return type; }

        private:
            Type type;
        };
    }
}
