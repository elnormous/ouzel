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
            if (!MeshBufferResource::upload())
            {
                return false;
            }

            switch (uploadData.indexSize)
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
    } // namespace graphics
} // namespace ouzel
