// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "VertexBufferVoid.h"

namespace ouzel
{
    namespace graphics
    {
        VertexBufferVoid::VertexBufferVoid()
        {
        }

        VertexBufferVoid::~VertexBufferVoid()
        {
        }

        bool VertexBufferVoid::upload()
        {
            ready = true;
            dirty = 0;
            return true;
        }
    }
}
