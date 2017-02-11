// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#define NOMINMAX
#include <d3d11.h>
#include "graphics/BufferResource.h"

namespace ouzel
{
    namespace graphics
    {
        class BufferD3D11;

        class BufferD3D11: public BufferResource
        {
        public:
            BufferD3D11();
            virtual ~BufferD3D11();

            ID3D11Buffer* getBuffer() const { return buffer; }

            BufferD3D11* getIndexBufferD3D11() const { return indexBufferD3D11; }
            BufferD3D11* getVertexBufferD3D11() const { return vertexBufferD3D11; }

        protected:
            virtual bool upload() override;

            ID3D11Buffer* buffer = nullptr;
            UINT bufferSize = 0;

            BufferD3D11* indexBufferD3D11 = nullptr;
            BufferD3D11* vertexBufferD3D11 = nullptr;
        };
    } // namespace graphics
} // namespace ouzel
