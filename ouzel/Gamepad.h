// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>

namespace ouzel
{
    class Gamepad
    {
    public:
        virtual bool isAttached() const;
        virtual void setAbsoluteDpadValues(bool absoluteDpadValues);
        virtual bool isAbsoluteDpadValues() const;
        
        virtual int32_t getPlayerIndex() const;
        virtual void setPlayerIndex(int32_t playerIndex);
    };
}
