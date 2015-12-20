// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Noncopyable.h"
#include "ReferenceCounted.h"

namespace ouzel
{
    class Renderer;
    
    class Texture: public Noncopyable, public ReferenceCounted
    {
    public:
        Texture(const std::string& filename, Renderer* renderer);
        virtual ~Texture();
        
        const std::string& getFilename() const { return _filename; }
        
        GLuint getTexture() const { return _texture; }
        
        GLsizei getWidth() const { return _width; }
        GLsizei getHeight() const { return _height; }
        
    private:
        Renderer* _renderer;
        std::string _filename;
        
        GLuint _texture = 0;
        GLsizei _width;
        GLsizei _height;
    };
}
