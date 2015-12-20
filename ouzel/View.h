// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Noncopyable.h"
#include "ReferenceCounted.h"
#include "Size2.h"

namespace ouzel
{
    class Renderer;
    
    class View: public Noncopyable, public ReferenceCounted
    {
    public:
        View(const Size2& size, Renderer* renderer);
        virtual ~View();
        
        Renderer* getRenderer() const { return _renderer; }
        
        void resize(const Size2& size);
        const Size2& getSize() const { return _size; }
        
        void* getNativeView() const { return _nativeView; }
        
    protected:
        void createNativeView();
        void destroyNativeView();
        
        Renderer* _renderer;
        Size2 _size;
        
        void* _nativeView = nullptr;
    };
}
