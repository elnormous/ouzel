// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "ShaderVoid.h"

namespace ouzel
{
    namespace graphics
    {
        ShaderVoid::ShaderVoid()
        {
        }

        ShaderVoid::~ShaderVoid()
        {
        }

        bool ShaderVoid::upload()
        {
            ready = true;
            dirty = false;
            return true;
        }
    } // namespace graphics
} // namespace ouzel
