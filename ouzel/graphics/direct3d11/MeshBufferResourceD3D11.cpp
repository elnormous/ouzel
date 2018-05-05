// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include "MeshBufferResourceD3D11.hpp"
#include "BufferResourceD3D11.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace graphics
    {
        MeshBufferResourceD3D11::MeshBufferResourceD3D11()
        {
        }

        MeshBufferResourceD3D11::~MeshBufferResourceD3D11()
        {
        }

        bool MeshBufferResourceD3D11::init(uint32_t newIndexSize, BufferResource* newIndexBuffer,
                                           BufferResource* newVertexBuffer)
        {
            if (!MeshBufferResource::init(newIndexSize, newIndexBuffer, newVertexBuffer))
                return false;

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

            indexBufferD3D11 = static_cast<BufferResourceD3D11*>(indexBuffer);
            vertexBufferD3D11 = static_cast<BufferResourceD3D11*>(vertexBuffer);

            return true;
        }

        bool MeshBufferResourceD3D11::setIndexSize(uint32_t newIndexSize)
        {
            if (MeshBufferResource::setIndexSize(newIndexSize))
                return false;

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

        bool MeshBufferResourceD3D11::setIndexBuffer(BufferResource* newIndexBuffer)
        {
            if (MeshBufferResource::setIndexBuffer(newIndexBuffer))
                return false;

            indexBufferD3D11 = static_cast<BufferResourceD3D11*>(indexBuffer);

            return true;
        }

        bool MeshBufferResourceD3D11::setVertexBuffer(BufferResource* newVertexBuffer)
        {
            if (MeshBufferResource::setVertexBuffer(newVertexBuffer))
                return false;

            vertexBufferD3D11 = static_cast<BufferResourceD3D11*>(vertexBuffer);

            return true;
        }
    } // namespace graphics
} // namespace ouzel

#endif
