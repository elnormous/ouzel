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
        friend Renderer;
    public:
        virtual ~Texture();
        
        virtual bool initFromFile(const std::string& filename);
        
        const std::string& getFilename() const { return _filename; }
        
        const Size2& getSize() const { return _size; }
        
    protected:
        Texture();
        
        std::string _filename;
        
        Size2 _size;
    };
}
