// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "graphics/IndexBuffer.h"

namespace ouzel
{
    namespace graphics
    {
        class IndexBufferEmpty: public IndexBuffer
        {
        public:
            IndexBufferEmpty();

        protected:
            virtual bool upload() override;
        };
    }
}
