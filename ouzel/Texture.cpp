// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Texture.h"

namespace ouzel
{
    Texture::Texture(const std::string& filename, Renderer* renderer)
    {
        _renderer = renderer;
        _filename = filename;
    }

    Texture::~Texture()
    {
        
    }
}
