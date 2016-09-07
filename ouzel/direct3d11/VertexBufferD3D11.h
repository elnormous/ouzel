// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <mutex>
#include <atomic>
#define NOMINMAX
#include <d3d11.h>
#include "graphics/VertexBuffer.h"

namespace ouzel
{
    namespace graphics
    {
        class RendererD3D11;
        
        class VertexBufferD3D11: public VertexBuffer
        {
            friend RendererD3D11;
        public:
            virtual ~VertexBufferD3D11();
            virtual void free() override;

            ID3D11Buffer* getBuffer() const { return buffer; }

        protected:
            VertexBufferD3D11();
            virtual bool upload() override;

            ID3D11Buffer* buffer = nullptr;
            UINT bufferSize = 0;
        };
    }
}
