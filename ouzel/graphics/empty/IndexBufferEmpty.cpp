// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "IndexBufferEmpty.h"

namespace ouzel
{
    namespace graphics
    {
        IndexBufferEmpty::IndexBufferEmpty()
        {
        }

        IndexBufferEmpty::~IndexBufferEmpty()
        {
        }


        bool IndexBufferEmpty::upload()
        {
            ready = true;
            dirty = 0;
            return true;
        }
    }
}
