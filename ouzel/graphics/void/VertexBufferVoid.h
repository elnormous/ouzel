// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "graphics/VertexBuffer.h"

namespace ouzel
{
    namespace graphics
    {
        class RendererVoid;

        class VertexBufferVoid: public VertexBuffer
        {
            friend RendererVoid;
        public:
            virtual ~VertexBufferVoid();

        protected:
            VertexBufferVoid();
            virtual bool upload() override;
        };
    }
}
