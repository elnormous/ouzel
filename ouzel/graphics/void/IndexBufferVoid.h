// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "graphics/IndexBuffer.h"

namespace ouzel
{
    namespace graphics
    {
        class RendererVoid;
        
        class IndexBufferVoid: public IndexBuffer
        {
            friend RendererVoid;
        public:
            virtual ~IndexBufferVoid();

        protected:
            IndexBufferVoid();
            virtual bool upload() override;
        };
    }
}
