// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include "Noncopyable.h"
#include "ReferenceCounted.h"
#include "Size2.h"

namespace ouzel
{
    class Engine;
    
    class Image: public Noncopyable, public ReferenceCounted
    {
    public:
        Image();
        virtual ~Image();
        
        const Size2& getSize() const { return _size; }
        const void* getData() const { return _data; }
        
        virtual bool loadFromFile(const std::string& filename);
        
    protected:
        std::string _filename;
        Size2 _size;
        
        void* _data = nullptr;
    };
}
