// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "graphics/VertexBuffer.h"

namespace ouzel
{
    namespace graphics
    {
        class RendererEmpty;

        class VertexBufferEmpty: public VertexBuffer
        {
            friend RendererEmpty;
        public:
            virtual ~VertexBufferEmpty();

        protected:
            VertexBufferEmpty();
            virtual bool upload() override;
        };
    }
}
