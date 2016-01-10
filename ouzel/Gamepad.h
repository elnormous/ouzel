// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include "Noncopyable.h"
#include "Event.h"

namespace ouzel
{
    class Gamepad: public Noncopyable, public std::enable_shared_from_this<Gamepad>
    {
    public:
        virtual bool isAttached() const;
        virtual void setAbsoluteDpadValues(bool absoluteDpadValues);
        virtual bool isAbsoluteDpadValues() const;
        
        virtual int32_t getPlayerIndex() const;
        virtual bool setPlayerIndex(int32_t playerIndex);
        
    protected:
        void handleButtonValueChange(GamepadButton button, bool pressed, float value);
    };
}
