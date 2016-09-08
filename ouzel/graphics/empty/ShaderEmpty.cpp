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

        ShaderEmpty::~ShaderEmpty()
        {
        }

        bool ShaderEmpty::upload()
        {
            ready = true;
            dirty = false;
            return true;
        }
    } // namespace graphics
} // namespace ouzel
