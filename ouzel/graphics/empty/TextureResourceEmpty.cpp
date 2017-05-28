// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "TextureResourceEmpty.h"

namespace ouzel
{
    namespace graphics
    {
        TextureResourceEmpty::TextureResourceEmpty()
        {
        }

        bool TextureResourceEmpty::upload()
        {
            return true;
        }
    } // namespace graphics
} // namespace ouzel
