// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBufferD3D11.h"
#include "utils/Log.h" 

namespace ouzel
{
    namespace graphics
    {
        MeshBufferD3D11::MeshBufferD3D11()
        {
        }

        MeshBufferD3D11::~MeshBufferD3D11()
        {
        }

        bool MeshBufferD3D11::upload()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            if (dirty)
            {
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

                indexBufferD3D11 = static_cast<BufferD3D11*>(indexBuffer);
                vertexBufferD3D11 = static_cast<BufferD3D11*>(vertexBuffer);

                dirty = 0;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
