// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Texture.h"

namespace ouzel
{
    Texture::Texture(Renderer* renderer)
    {
        _renderer = renderer;
    }

    Texture::~Texture()
    {
        
    }
    
    bool Texture::loadFromFile(const std::string& filename)
    {
        _filename = filename;
        
        return true;
    }
}
