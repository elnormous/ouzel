// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.hpp"

#if OUZEL_SUPPORTS_METAL

#include "MeshBufferInterfaceMetal.hpp"
#include "RendererMetal.hpp"
#include "BufferInterfaceMetal.hpp"
#include "core/Engine.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace graphics
    {
        MeshBufferInterfaceMetal::MeshBufferInterfaceMetal()
        {
        }

        MeshBufferInterfaceMetal::~MeshBufferInterfaceMetal()
        {
        }

        bool MeshBufferInterfaceMetal::init(uint32_t newIndexSize, BufferInterface* newIndexBuffer,
                                           const std::vector<VertexAttribute>& newVertexAttributes, BufferInterface* newVertexBuffer)
        {
            if (!MeshBufferInterface::init(newIndexSize, newIndexBuffer, newVertexAttributes, newVertexBuffer))
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

            indexBufferMetal = static_cast<BufferInterfaceMetal*>(indexBuffer);
            vertexBufferMetal = static_cast<BufferInterfaceMetal*>(vertexBuffer);

            return true;
        }

        bool MeshBufferInterfaceMetal::setIndexSize(uint32_t newIndexSize)
        {
            if (MeshBufferInterface::setIndexSize(newIndexSize))
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

        bool MeshBufferInterfaceMetal::setIndexBuffer(BufferInterface* newIndexBuffer)
        {
            if (MeshBufferInterface::setIndexBuffer(newIndexBuffer))
            {
                return false;
            }

            indexBufferMetal = static_cast<BufferInterfaceMetal*>(indexBuffer);

            return true;
        }

        bool MeshBufferInterfaceMetal::setVertexAttributes(const std::vector<VertexAttribute>& newVertexAttributes)
        {
            if (MeshBufferInterface::setVertexAttributes(newVertexAttributes))
            {
                return false;
            }

            return true;
        }

        bool MeshBufferInterfaceMetal::setVertexBuffer(BufferInterface* newVertexBuffer)
        {
            if (MeshBufferInterface::setVertexBuffer(newVertexBuffer))
            {
                return false;
            }

            vertexBufferMetal = static_cast<BufferInterfaceMetal*>(vertexBuffer);

            return true;
        }
    } // namespace graphics
} // namespace ouzel

#endif
