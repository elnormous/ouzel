// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "TextureEmpty.h"

namespace ouzel
{
    namespace graphics
    {
        TextureEmpty::TextureEmpty()
        {
        }

        TextureEmpty::~TextureEmpty()
        {
        }

        bool TextureEmpty::upload()
        {
            ready = true;
            dirty = false;
            return true;
        }
    } // namespace graphics
} // namespace ouzel
