// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_INPUTDEVICE_HPP
#define OUZEL_INPUT_INPUTDEVICE_HPP

#include <cstdint>
#include "input/Controller.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystem;

        class InputDevice
        {
        public:
            InputDevice(InputSystem& initInputSystem, DeviceId initId, Controller::Type initType);
            virtual ~InputDevice();

            InputDevice(const InputDevice&) = delete;
            InputDevice& operator=(const InputDevice&) = delete;
            InputDevice(InputDevice&&) = delete;
            InputDevice& operator=(InputDevice&&) = delete;

            inline auto getId() const noexcept { return id; }
            inline auto getType() const noexcept { return type; }

        protected:
            InputSystem& inputSystem;
            DeviceId id;
            Controller::Type type;
        };
    } // namespace input
} // namespace ouzel

#endif // OUZEL_INPUT_INPUTDEVICE_HPP
