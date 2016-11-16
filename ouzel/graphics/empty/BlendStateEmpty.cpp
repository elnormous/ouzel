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

        bool BlendStateEmpty::upload()
        {
            uploadData.dirty = false;

            return true;
        }
    } // namespace graphics
} // namespace ouzel
