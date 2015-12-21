// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "Noncopyable.h"
#include "ReferenceCounted.h"
#include "Vertex.h"

namespace ouzel
{
    class Renderer;
    
    class MeshBuffer: public Noncopyable, public ReferenceCounted
    {
    public:
        MeshBuffer(Renderer* renderer);
        virtual ~MeshBuffer();
        
        virtual bool initFromData(const std::vector<uint16_t>& indices, const std::vector<Vertex>& vertices);
        
    protected:
        Renderer* _renderer;
    };
}
