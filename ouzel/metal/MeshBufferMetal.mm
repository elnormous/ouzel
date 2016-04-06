// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBufferMetal.h"
#include "Engine.h"
#include "RendererMetal.h"
#include "Utils.h"

namespace ouzel
{
    namespace video
    {
        MeshBufferMetal::MeshBufferMetal()
        {
            
        }
        
        MeshBufferMetal::~MeshBufferMetal()
        {
            destroy();
        }
        
        void MeshBufferMetal::destroy()
        {
            if (_indexBuffer) [_indexBuffer release];
            if (_vertexBuffer) [_vertexBuffer release];
        }
        
        bool MeshBufferMetal::initFromData(const void* indices, uint32_t indexSize, uint32_t indexCount, bool dynamicIndexBuffer, const void* vertices, uint32_t vertexSize, uint32_t vertexCount, bool dynamicVertexBuffer, uint32_t vertexAttributes)
        {
            if (!MeshBuffer::initFromData(indices, indexSize, indexCount, dynamicIndexBuffer, vertices, vertexSize, vertexCount, dynamicVertexBuffer, vertexAttributes))
            {
                return false;
            }

            destroy();
            
            return true;
        }
        
        bool MeshBufferMetal::uploadIndices(const void* indices, uint32_t indexCount)
        {
            if (!MeshBuffer::uploadIndices(indices, indexCount))
            {
                return false;
            }
            
            return true;
        }
        
        bool MeshBufferMetal::uploadVertices(const void* vertices, uint32_t vertexCount)
        {
            if (!MeshBuffer::uploadVertices(vertices, vertexCount))
            {
                return false;
            }
            
            return true;
        }
    } // namespace video
} // namespace ouzel
