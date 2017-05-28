// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "ShaderResourceEmpty.h"

namespace ouzel
{
    namespace graphics
    {
        ShaderResourceEmpty::ShaderResourceEmpty()
        {
        }

        bool ShaderResourceEmpty::upload()
        {
            return true;
        }
    } // namespace graphics
} // namespace ouzel
