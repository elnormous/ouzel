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
    
    bool TextureOGL::initFromData(const void* data, const Size2& size, bool dynamic)
    {
        if (!Texture::initFromData(data, size, dynamic))
        {
            return false;
        }
        
        glGenTextures(1, &_textureId);
        
        glBindTexture(GL_TEXTURE_2D, _textureId);
        
        uploadData(data,
                   static_cast<GLsizei>(size.width),
                   static_cast<GLsizei>(size.height));
        
        return true;
    }
    
    bool TextureOGL::upload(const void* data, const Size2& size)
    {
        if (!Texture::upload(data, size))
        {
            return false;
        }
        
        return uploadData(data, static_cast<GLsizei>(size.width), static_cast<GLsizei>(size.height));
    }
    
    bool TextureOGL::uploadData(const void* data, GLsizei width, GLsizei height)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        
        if (std::static_pointer_cast<RendererOGL>(Engine::getInstance()->getRenderer())->checkOpenGLErrors())
        {
            return false;
        }
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        if (std::static_pointer_cast<RendererOGL>(Engine::getInstance()->getRenderer())->checkOpenGLErrors())
        {
            return false;
        }
        
        // Mipmaps are supported only for power-of-two sized textures
        if (isPOT(static_cast<int>(width)) &&
            isPOT(static_cast<int>(height)))
        {
            glGenerateMipmap(GL_TEXTURE_2D);
            
            if (std::static_pointer_cast<RendererOGL>(Engine::getInstance()->getRenderer())->checkOpenGLErrors())
            {
                return false;
            }
        }
        
        glBindTexture(GL_TEXTURE_2D, 0);
        
        return true;
    }
}
