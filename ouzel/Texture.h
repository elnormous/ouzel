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
        
        uint32_t getWidth() const { return _width; }
        uint32_t getHeight() const { return _height; }
        
    protected:
        Renderer* _renderer;
        std::string _filename;
        
        uint32_t _width;
        uint32_t _height;
    };
}
