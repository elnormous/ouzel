// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Gamepad.h"

#if defined(__OBJC__)
@class GCController;
typedef GCController* GCControllerPtr;
#else
#include <objc/objc.h>
typedef id GCControllerPtr;
#endif

namespace ouzel
{
    namespace input
    {
        class InputApple;

        class GamepadApple: public Gamepad
        {
            friend InputApple;
        public:
            virtual bool isAttached() const override;
            virtual void setAbsoluteDpadValues(bool absoluteDpadValues) override;
            virtual bool isAbsoluteDpadValues() const override;

            virtual int32_t getPlayerIndex() const override;
            virtual bool setPlayerIndex(int32_t playerIndex) override;

            GCControllerPtr getController() const { return controller; }

        protected:
            GamepadApple(GCControllerPtr pController);

            GCControllerPtr controller;
        };
    } // namespace input
} // namespace ouzel
