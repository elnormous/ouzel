// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include "Noncopyable.h"
#include "ReferenceCounted.h"
#include "Size2.h"

namespace ouzel
{
    class Renderer;
    
    class Texture: public Noncopyable, public ReferenceCounted
    {
    public:
        Texture(const std::string& filename, Renderer* renderer);
        virtual ~Texture();
        
        const std::string& getFilename() const { return _filename; }
        
        const Size2& getSize() const { return _size; }
        
    protected:
        Renderer* _renderer;
        std::string _filename;
        
        Size2 _size;
    };
}
