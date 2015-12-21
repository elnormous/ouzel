// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Noncopyable.h"
#include "ReferenceCounted.h"

namespace ouzel
{
    class SoundManager;
    
    class Sound: public Noncopyable, public ReferenceCounted
    {
    public:
        Sound(SoundManager* soundManager);
        virtual ~Sound();
        
    protected:
        SoundManager* _soundManager;
    };
}
