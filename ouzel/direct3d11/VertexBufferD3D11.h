// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

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

        protected:
            VertexBufferD3D11();
        };
    }
}
