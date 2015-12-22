// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "CompileConfig.h"
#include "Noncopyable.h"
#include "ReferenceCounted.h"
#include "Size2.h"

#ifdef OUZEL_PLATFORM_OSX
#ifdef __OBJC__
@class NSView;
#else
class NSView;
#endif
#endif

namespace ouzel
{
    class Renderer;
    
    class View: public Noncopyable, public ReferenceCounted
    {
    public:
        View(const Size2& size, Renderer* renderer);
        virtual ~View();
        
        Renderer* getRenderer() const { return _renderer; }
        
        virtual void lock();
        
        virtual bool processEvents();
        virtual void resize(const Size2& size);
        const Size2& getSize() const { return _size; }
        
#if defined(OUZEL_PLATFORM_OSX)
        NSView* getNativeView() const { return _nativeView; }
#endif
        
    protected:
        virtual void createNativeView();
        virtual void destroyNativeView();
        
        Renderer* _renderer;
        Size2 _size;
        
#if defined(OUZEL_PLATFORM_OSX)
        NSView* _nativeView = nullptr;
#endif
        
    };
}
