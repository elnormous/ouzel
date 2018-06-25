// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#if defined(__OBJC__)
#include <GameController/GameController.h>
typedef GCController* GCControllerPtr;
#else
#include <objc/objc.h>
typedef id GCControllerPtr;
#endif

#include "input/Gamepad.hpp"

namespace ouzel
{
    namespace input
    {
        class GamepadTVOS: public Gamepad
        {
        public:
            explicit GamepadTVOS(GCControllerPtr initController);
            virtual void setAbsoluteDpadValues(bool absoluteDpadValues) override;
            virtual bool isAbsoluteDpadValues() const override;

            virtual int32_t getPlayerIndex() const override;
            virtual bool setPlayerIndex(int32_t playerIndex) override;

            inline GCControllerPtr getController() const { return controller; }

        private:
            GCControllerPtr controller;
        };
    } // namespace input
} // namespace ouzel
