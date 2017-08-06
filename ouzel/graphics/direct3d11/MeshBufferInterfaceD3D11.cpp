// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_DIRECT3D11

#include "MeshBufferInterfaceD3D11.hpp"
#include "BufferInterfaceD3D11.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace graphics
    {
        MeshBufferInterfaceD3D11::MeshBufferInterfaceD3D11()
        {
        }

        MeshBufferInterfaceD3D11::~MeshBufferInterfaceD3D11()
        {
        }

        bool MeshBufferInterfaceD3D11::init(uint32_t newIndexSize, BufferInterface* newIndexBuffer,
                                            const std::vector<VertexAttribute>& newVertexAttributes, BufferInterface* newVertexBuffer)
        {
            if (!MeshBufferInterface::init(newIndexSize, newIndexBuffer, newVertexAttributes, newVertexBuffer))
            {
                return false;
            }

            switch (indexSize)
            {
                case 2:
                    indexFormat = DXGI_FORMAT_R16_UINT;
                    break;
                case 4:
                    indexFormat = DXGI_FORMAT_R32_UINT;
                    break;
                default:
                    indexFormat = DXGI_FORMAT_UNKNOWN;
                    Log(Log::Level::ERR) << "Invalid index size";
                    return false;
            }

            indexBufferD3D11 = static_cast<BufferInterfaceD3D11*>(indexBuffer);
            vertexBufferD3D11 = static_cast<BufferInterfaceD3D11*>(vertexBuffer);

            return true;
        }

        bool MeshBufferInterfaceD3D11::setIndexSize(uint32_t newIndexSize)
        {
            if (MeshBufferInterface::setIndexSize(newIndexSize))
            {
                return false;
            }

            switch (indexSize)
            {
                case 2:
                    indexFormat = DXGI_FORMAT_R16_UINT;
                    break;
                case 4:
                    indexFormat = DXGI_FORMAT_R32_UINT;
                    break;
                default:
                    indexFormat = DXGI_FORMAT_UNKNOWN;
                    Log(Log::Level::ERR) << "Invalid index size";
                    return false;
            }

            return true;
        }

        bool MeshBufferInterfaceD3D11::setIndexBuffer(BufferInterface* newIndexBuffer)
        {
            if (MeshBufferInterface::setIndexBuffer(newIndexBuffer))
            {
                return false;
            }

            indexBufferD3D11 = static_cast<BufferInterfaceD3D11*>(indexBuffer);

            return true;
        }

        bool MeshBufferInterfaceD3D11::setVertexAttributes(const std::vector<VertexAttribute>& newVertexAttributes)
        {
            if (MeshBufferInterface::setVertexAttributes(newVertexAttributes))
            {
                return false;
            }

            return true;
        }

        bool MeshBufferInterfaceD3D11::setVertexBuffer(BufferInterface* newVertexBuffer)
        {
            if (MeshBufferInterface::setVertexBuffer(newVertexBuffer))
            {
                return false;
            }

            vertexBufferD3D11 = static_cast<BufferInterfaceD3D11*>(vertexBuffer);

            return true;
        }
    } // namespace graphics
} // namespace ouzel

#endif
