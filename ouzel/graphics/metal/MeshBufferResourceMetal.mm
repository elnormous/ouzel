// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

#include "MeshBufferResourceMetal.hpp"
#include "RenderDeviceMetal.hpp"
#include "BufferResourceMetal.hpp"
#include "core/Engine.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace graphics
    {
        MeshBufferResourceMetal::MeshBufferResourceMetal()
        {
        }

        MeshBufferResourceMetal::~MeshBufferResourceMetal()
        {
        }

        bool MeshBufferResourceMetal::init(uint32_t newIndexSize, BufferResource* newIndexBuffer,
                                           const std::vector<Vertex::Attribute>& newVertexAttributes, BufferResource* newVertexBuffer)
        {
            if (!MeshBufferResource::init(newIndexSize, newIndexBuffer, newVertexAttributes, newVertexBuffer))
            {
                return false;
            }

            switch (indexSize)
            {
                case 2:
                    indexType = MTLIndexTypeUInt16;
                    bytesPerIndex = 2;
                    break;
                case 4:
                    indexType = MTLIndexTypeUInt32;
                    bytesPerIndex = 4;
                    break;
                default:
                    bytesPerIndex = 0;
                    Log(Log::Level::ERR) << "Invalid index size";
                    return false;
            }

            indexBufferMetal = static_cast<BufferResourceMetal*>(indexBuffer);
            vertexBufferMetal = static_cast<BufferResourceMetal*>(vertexBuffer);

            return true;
        }

        bool MeshBufferResourceMetal::setIndexSize(uint32_t newIndexSize)
        {
            if (MeshBufferResource::setIndexSize(newIndexSize))
            {
                return false;
            }

            switch (indexSize)
            {
                case 2:
                    indexType = MTLIndexTypeUInt16;
                    bytesPerIndex = 2;
                    break;
                case 4:
                    indexType = MTLIndexTypeUInt32;
                    bytesPerIndex = 4;
                    break;
                default:
                    bytesPerIndex = 0;
                    Log(Log::Level::ERR) << "Invalid index size";
                    return false;
            }

            return true;
        }

        bool MeshBufferResourceMetal::setIndexBuffer(BufferResource* newIndexBuffer)
        {
            if (MeshBufferResource::setIndexBuffer(newIndexBuffer))
            {
                return false;
            }

            indexBufferMetal = static_cast<BufferResourceMetal*>(indexBuffer);

            return true;
        }

        bool MeshBufferResourceMetal::setVertexAttributes(const std::vector<Vertex::Attribute>& newVertexAttributes)
        {
            if (MeshBufferResource::setVertexAttributes(newVertexAttributes))
            {
                return false;
            }

            return true;
        }

        bool MeshBufferResourceMetal::setVertexBuffer(BufferResource* newVertexBuffer)
        {
            if (MeshBufferResource::setVertexBuffer(newVertexBuffer))
            {
                return false;
            }

            vertexBufferMetal = static_cast<BufferResourceMetal*>(vertexBuffer);

            return true;
        }
    } // namespace graphics
} // namespace ouzel

#endif
