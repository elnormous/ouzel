// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "IndexBufferVoid.h"

namespace ouzel
{
    namespace graphics
    {
        IndexBufferVoid::IndexBufferVoid()
        {
        }

        IndexBufferVoid::~IndexBufferVoid()
        {
        }


        bool IndexBufferVoid::upload()
        {
            ready = true;
            dirty = 0;
            return true;
        }
    }
}
