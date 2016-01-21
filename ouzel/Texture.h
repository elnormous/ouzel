// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include "Noncopyable.h"
#include "Size2.h"

namespace ouzel
{
    class Renderer;
    
    class Texture: public Noncopyable
    {
        friend Renderer;
    public:
        virtual ~Texture();
        
        virtual bool init(const Size2& size, bool dynamic, bool mipmaps = true);
        virtual bool initFromFile(const std::string& filename, bool dynamic, bool mipmaps = true);
        virtual bool initFromData(const void* data, const Size2& size, bool dynamic, bool mipmaps = true);
        
        const std::string& getFilename() const { return _filename; }
        
        virtual bool upload(const void* data, const Size2& size);
        
        const Size2& getSize() const { return _size; }
        
        bool isDynamic() const { return _dynamic; }
        
    protected:
        Texture();
        
        std::string _filename;
        
        Size2 _size;
        bool _dynamic = false;
        bool _mipmaps = false;
    };
}
