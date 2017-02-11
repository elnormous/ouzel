// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#define NOMINMAX
#include <d3d11.h>
#include "graphics/MeshBufferResource.h"

namespace ouzel
{
    namespace graphics
    {
        class BufferD3D11;

        class MeshBufferD3D11: public MeshBufferResource
        {
        public:
            MeshBufferD3D11();
            virtual ~MeshBufferD3D11();

            DXGI_FORMAT getIndexFormat() const { return indexFormat; }

            BufferD3D11* getIndexBufferD3D11() const { return indexBufferD3D11; }
            BufferD3D11* getVertexBufferD3D11() const { return vertexBufferD3D11; }

        protected:
            virtual bool upload() override;

            DXGI_FORMAT indexFormat = DXGI_FORMAT_UNKNOWN;

            BufferD3D11* indexBufferD3D11 = nullptr;
            BufferD3D11* vertexBufferD3D11 = nullptr;
        };
    } // namespace graphics
} // namespace ouzel
