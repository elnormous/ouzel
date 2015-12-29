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
        friend Renderer;
    public:
        virtual ~MeshBuffer();
        
        virtual bool initFromData(const std::vector<uint16_t>& indices, const std::vector<Vertex>& vertices, bool dynamicIndexBuffer = false, bool dynamicVertexBuffer = false);
        
        virtual bool uploadIndices(const std::vector<uint16_t>& indices);
        virtual bool uploadVertices(const std::vector<Vertex>& vertices);
        
    protected:
        MeshBuffer(Renderer* renderer);
        
        Renderer* _renderer;
        bool _dynamicIndexBuffer = false;
        bool _dynamicVertexBuffer = false;
    };
}
