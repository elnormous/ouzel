// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "TextureVoid.h"

namespace ouzel
{
    namespace graphics
    {
        TextureVoid::TextureVoid()
        {
        }

        TextureVoid::~TextureVoid()
        {
        }

        bool TextureVoid::upload()
        {
            ready = true;
            dirty = false;
            return true;
        }
    } // namespace graphics
} // namespace ouzel
