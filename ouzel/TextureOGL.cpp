// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "TextureOGL.h"
#include "Engine.h"
#include "RendererOGL.h"
#include "Image.h"
#include "Utils.h"

namespace ouzel
{
    TextureOGL::TextureOGL()
    {
        
    }
    
    TextureOGL::~TextureOGL()
    {
        if (_textureId)
        {
            glDeleteTextures(1, &_textureId);
        }
    }
    
    bool TextureOGL::initFromFile(const std::string& filename)
    {
        if (!Texture::initFromFile(filename))
        {
            return false;
        }
        
        AutoPtr<Image> image = new Image();
        if (!image->loadFromFile(filename))
        {
            return false;
        }
        
        glGenTextures(1, &_textureId);
        
        glBindTexture(GL_TEXTURE_2D, _textureId);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->getSize().width, image->getSize().height,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, image->getData());
        
        if (static_cast<RendererOGL*>(Renderer::getInstance())->checkOpenGLErrors())
        {
            return false;
        }
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        if (static_cast<RendererOGL*>(Renderer::getInstance())->checkOpenGLErrors())
        {
            return false;
        }
        
        // Mipmaps are supported only for power-of-two sized textures
        if (isPOT(static_cast<int>(image->getSize().width)) &&
            isPOT(static_cast<int>(image->getSize().height)))
        {
            glGenerateMipmap(GL_TEXTURE_2D);
        
            if (static_cast<RendererOGL*>(Renderer::getInstance())->checkOpenGLErrors())
            {
                return false;
            }
        }
        
        glBindTexture(GL_TEXTURE_2D, 0);
        
        _size = image->getSize();
        
        return true;
    }
}
