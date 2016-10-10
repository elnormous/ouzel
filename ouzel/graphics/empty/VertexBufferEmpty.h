// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "graphics/VertexBuffer.h"

namespace ouzel
{
    namespace graphics
    {
        class VertexBufferEmpty: public VertexBuffer
        {
        public:
            VertexBufferEmpty();
            virtual ~VertexBufferEmpty();

        protected:
            virtual bool upload() override;
        };
    }
}
