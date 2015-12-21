// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "TextureOGL.h"
#include "Engine.h"
#include "Image.h"
#include "Utils.h"

namespace ouzel
{
    TextureOGL::TextureOGL(Renderer* renderer):
        Texture(renderer)
    {
        
    }
    
    TextureOGL::~TextureOGL()
    {
        if (_textureId)
        {
            glDeleteTextures(1, &_textureId);
        }
    }
    
    bool TextureOGL::loadFromFile(const std::string& filename)
    {
        if (!Texture::loadFromFile(filename))
        {
            return false;
        }
        
        Image* image = new Image(_renderer->getEngine());
        if (!image->loadFromFile(filename))
        {
            return false;
        }
        
        glGenTextures(1, &_textureId);
        
        glBindTexture(GL_TEXTURE_2D, _textureId);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->getSize().width, image->getSize().height,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, image->getData());
        
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        glGenerateMipmap(GL_TEXTURE_2D);
        
        checkOpenGLErrors();
        
        _size = image->getSize();
        
        image->release();
        
        return true;
    }
}
