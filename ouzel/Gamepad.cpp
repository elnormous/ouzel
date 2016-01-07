// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Gamepad.h"

namespace ouzel
{
    bool Gamepad::isAttached() const
    {
        return false;
    }
    
    void Gamepad::setAbsoluteDpadValues(bool absoluteDpadValues)
    {
        
    }
    
    bool Gamepad::isAbsoluteDpadValues() const
    {
        return false;
    }
    
    int32_t Gamepad::getPlayerIndex() const
    {
        return -1;
    }
    
    void Gamepad::setPlayerIndex(int32_t playerIndex)
    {
        
    }
}
