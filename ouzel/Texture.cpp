// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Texture.h"
#include "Renderer.h"

namespace ouzel
{
    Texture::Texture(Renderer* renderer):
        _renderer(renderer)
    {
        
    }

    Texture::~Texture()
    {
        for (int i = 0; i < TEXTURE_LAYERS; ++i)
        {
            if (_renderer->getActiveTexture(i) == this)
            {
                _renderer->activateTexture(nullptr, i);
            }
        }
    }
    
    bool Texture::initFromFile(const std::string& filename)
    {
        _filename = filename;
        
        return true;
    }
}
