// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "graphics/IndexBuffer.h"

namespace ouzel
{
    namespace graphics
    {
        class RendererMetal;
        
        class IndexBufferMetal: public IndexBuffer
        {
            friend RendererMetal;
        public:
            virtual ~IndexBufferMetal();
            virtual void free() override;

        protected:
            IndexBufferMetal();
        };
    }
}
