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
                Keyboard,
                Mouse,
                Touchpad,
                Gamepad
            };

            Controller(InputManager& initInputManager, Type initType, uintptr_t initDeviceId):
                inputManager(initInputManager), type(initType), deviceId(initDeviceId)
            {}
            virtual ~Controller() = default;

            inline auto getType() const noexcept { return type; }
            inline auto getDeviceId() const noexcept { return deviceId; }

        protected:
            InputManager& inputManager;
            Type type;
            uintptr_t deviceId;
        };
    } // namespace input
} // namespace ouzel

#endif // OUZEL_INPUT_CONTROLLER_HPP
