// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_DIRECT3D11

#include <d3d11.h>
#include "graphics/MeshBufferResource.h"

namespace ouzel
{
    namespace graphics
    {
        class BufferResourceD3D11;

        class MeshBufferResourceD3D11: public MeshBufferResource
        {
        public:
            MeshBufferResourceD3D11();
            virtual ~MeshBufferResourceD3D11();

            DXGI_FORMAT getIndexFormat() const { return indexFormat; }

            BufferResourceD3D11* getIndexBufferD3D11() const { return indexBufferD3D11; }
            BufferResourceD3D11* getVertexBufferD3D11() const { return vertexBufferD3D11; }

        protected:
            virtual bool upload() override;

            DXGI_FORMAT indexFormat = DXGI_FORMAT_UNKNOWN;

            BufferResourceD3D11* indexBufferD3D11 = nullptr;
            BufferResourceD3D11* vertexBufferD3D11 = nullptr;
        };
    } // namespace graphics
} // namespace ouzel

#endif
