// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "ShaderEmpty.h"

namespace ouzel
{
    namespace graphics
    {
        ShaderEmpty::ShaderEmpty()
        {
        }

        bool ShaderEmpty::upload()
        {
            uploadData.dirty = false;

            return true;
        }
    } // namespace graphics
} // namespace ouzel
