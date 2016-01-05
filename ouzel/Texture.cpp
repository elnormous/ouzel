// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Texture.h"
#include "Engine.h"
#include "Renderer.h"
#include "Image.h"

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
    
    bool Texture::initFromFile(const std::string& filename, bool dynamic)
    {
        _filename = filename;
        _dynamic = dynamic;

        AutoPtr<Image> image = new Image();
        if (!image->loadFromFile(filename))
        {
            return false;
        }

        return initFromData(image->getData(), image->getSize(), dynamic);
    }

    bool Texture::initFromData(const void* data, const Size2& size, bool dynamic)
    {
        _dynamic = dynamic;
        _size = size;

        return true;
    }
    
    bool Texture::upload(const void* data, const Size2& size)
    {
        _size = size;
        
        if (!_dynamic)
        {
            return false;
        }
        
        return true;
    }
}
