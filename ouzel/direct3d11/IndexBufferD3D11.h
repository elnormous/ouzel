// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <mutex>
#include <atomic>
#define NOMINMAX
#include <d3d11.h>
#include "graphics/IndexBuffer.h"

namespace ouzel
{
    namespace graphics
    {
        class RendererD3D11;
        
        class IndexBufferD3D11: public IndexBuffer
        {
            friend RendererD3D11;
        public:
            virtual ~IndexBufferD3D11();
            virtual void free() override;

            ID3D11Buffer* getBuffer() const { return buffer; }
            DXGI_FORMAT getFormat() const { return format; }

        protected:
            IndexBufferD3D11();
            virtual bool upload() override;

            ID3D11Buffer* buffer = nullptr;
            UINT bufferSize = 0;
            DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
        };
    }
}
