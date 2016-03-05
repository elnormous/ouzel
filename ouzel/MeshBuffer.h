// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "Noncopyable.h"
#include "Vertex.h"

namespace ouzel
{
    class Renderer;
    
    class MeshBuffer: public Noncopyable
    {
        friend Renderer;
    public:
        virtual ~MeshBuffer();
        
        virtual bool initFromData(const void* indices, uint32_t indexSize, uint32_t indexCount, bool dynamicIndexBuffer, const void* vertices, uint32_t vertexSize, uint32_t vertexCount, bool dynamicVertexBuffer, uint32_t vertexAttributes);
        
        uint32_t getIndexCount() const { return _indexCount; }
        uint32_t getIndexSize() const { return _indexSize; }
        uint32_t getVertexCount() const { return _vertexCount; }
        uint32_t getVertexSize() const { return _vertexSize; }

        uint32_t getVertexAttributes() const { return _vertexAttributes; }
        
        virtual bool uploadIndices(const void* indices, uint32_t indexCount);
        virtual bool uploadVertices(const void* vertices, uint32_t vertexCount);
        
    protected:
        MeshBuffer();
        
        uint32_t _indexCount = 0;
        uint32_t _indexSize = 0;
        bool _dynamicIndexBuffer = false;
        
        uint32_t _vertexCount = 0;
        uint32_t _vertexSize = 0;
        bool _dynamicVertexBuffer = false;
        
        
        uint32_t _vertexAttributes;
    };
}
