// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#define NOMINMAX
#include <d3d11.h>
#include "graphics/IndexBufferResource.h"

namespace ouzel
{
    namespace graphics
    {
        class IndexBufferD3D11: public IndexBufferResource
        {
        public:
            IndexBufferD3D11();
            virtual ~IndexBufferD3D11();
            virtual void free() override;

            ID3D11Buffer* getBuffer() const { return buffer; }

        protected:
            virtual bool upload() override;

            ID3D11Buffer* buffer = nullptr;
            UINT bufferSize = 0;
        };
    } // namespace graphics
} // namespace ouzel
