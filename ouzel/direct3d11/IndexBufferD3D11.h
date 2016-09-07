// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

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

        protected:
            IndexBufferD3D11();
        };
    }
}
