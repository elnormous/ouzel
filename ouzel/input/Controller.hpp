// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_CONTROLLER_HPP
#define OUZEL_INPUT_CONTROLLER_HPP

namespace ouzel
{
    namespace input
    {
        class InputManager;

        class Controller
        {
        public:
            enum class Type
            {
                KEYBOARD,
                MOUSE,
                TOUCHPAD,
                GAMEPAD
            };

            explicit Controller(InputManager& initInputManager, Type initType, uint32_t initDeviceId):
                inputManager(initInputManager), type(initType), deviceId(initDeviceId)
            {}
            virtual ~Controller() {}

            inline Type getType() const { return type; }
            inline uint32_t getDeviceId() const { return deviceId; }

        protected:
            InputManager& inputManager;
            Type type;
            uint32_t deviceId;
        };
    } // namespace input
} // namespace ouzel

#endif // OUZEL_INPUT_CONTROLLER_HPP
