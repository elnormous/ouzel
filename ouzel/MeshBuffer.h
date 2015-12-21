// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Noncopyable.h"
#include "ReferenceCounted.h"

namespace ouzel
{
    class Renderer;
    
    class MeshBuffer: public Noncopyable, public ReferenceCounted
    {
    public:
        MeshBuffer(Renderer* renderer);
        virtual ~MeshBuffer();
        
    protected:
        Renderer* _renderer;
    };
}
