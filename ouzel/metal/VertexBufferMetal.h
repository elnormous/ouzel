// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "graphics/VertexBuffer.h"

namespace ouzel
{
    namespace graphics
    {
        class RendererMetal;
        
        class VertexBufferMetal: public VertexBuffer
        {
            friend RendererMetal;
        public:
            virtual ~VertexBufferMetal();
            virtual void free() override;

        protected:
            VertexBufferMetal();
        };
    }
}
