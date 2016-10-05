// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "graphics/IndexBuffer.h"

namespace ouzel
{
    namespace graphics
    {
        class RendererEmpty;

        class IndexBufferEmpty: public IndexBuffer
        {
            friend RendererEmpty;
        public:
            virtual ~IndexBufferEmpty();

        protected:
            IndexBufferEmpty();
            virtual bool upload() override;
        };
    }
}
