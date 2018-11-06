// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GAMEPADDEVICEIOS_HPP
#define OUZEL_GAMEPADDEVICEIOS_HPP

#include <string>
#if defined(__OBJC__)
#include <GameController/GameController.h>
typedef GCController* GCControllerPtr;
#else
#include <objc/objc.h>
typedef id GCControllerPtr;
#endif

#include "input/GamepadDevice.hpp"
#include "input/Gamepad.hpp"

namespace ouzel
{
    namespace input
    {
        class GamepadDeviceIOS final: public GamepadDevice
        {
        public:
            GamepadDeviceIOS(InputSystem& initInputSystem,
                             uint32_t initId,
                             GCControllerPtr initController);

            void setAbsoluteDPadValues(bool absoluteDPadValues);
            bool isAbsoluteDPadValues() const;

            int32_t getPlayerIndex() const;
            void setPlayerIndex(int32_t playerIndex);

            GCControllerPtr getController() const { return controller; }

        private:
            GCControllerPtr controller;
            std::string name;
            bool attached = false;
        };
    } // namespace input
} // namespace ouzel

#endif // OUZEL_GAMEPADDEVICEIOS_HPP
