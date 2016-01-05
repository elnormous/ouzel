// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Texture.h"
#include "Engine.h"
#include "Renderer.h"

namespace ouzel
{
    Texture::Texture()
    {
        
    }

    Texture::~Texture()
    {
        for (int i = 0; i < Renderer::TEXTURE_LAYERS; ++i)
        {
            if (Renderer::getInstance() && Renderer::getInstance()->getActiveTexture(i) == this)
            {
                Renderer::getInstance()->activateTexture(nullptr, i);
            }
        }
    }
    
    bool Texture::initFromFile(const std::string& filename)
    {
        _filename = filename;
        
        return true;
    }
}
