// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "BlendStateVoid.h"

namespace ouzel
{
    namespace graphics
    {
        BlendStateVoid::BlendStateVoid()
        {
        }

        BlendStateVoid::~BlendStateVoid()
        {
        }

        bool BlendStateVoid::upload()
        {
            ready = true;
            dirty = false;

            return true;
        }
    } // namespace graphics
} // namespace ouzel
