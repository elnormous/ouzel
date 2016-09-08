// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "VertexBufferEmpty.h"

namespace ouzel
{
    namespace graphics
    {
        VertexBufferEmpty::VertexBufferEmpty()
        {
        }

        VertexBufferEmpty::~VertexBufferEmpty()
        {
        }

        bool VertexBufferEmpty::upload()
        {
            ready = true;
            dirty = 0;
            return true;
        }
    }
}
