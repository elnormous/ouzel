// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "BlendStateEmpty.h"

namespace ouzel
{
    namespace graphics
    {
        BlendStateEmpty::BlendStateEmpty()
        {
        }

        BlendStateEmpty::~BlendStateEmpty()
        {
        }

        bool BlendStateEmpty::upload()
        {
            ready = true;
            dirty = false;

            return true;
        }
    } // namespace graphics
} // namespace ouzel
