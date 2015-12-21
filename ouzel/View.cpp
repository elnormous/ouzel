// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "View.h"
#include "Engine.h"

namespace ouzel
{
    View::View(const Size2& size, Renderer* renderer):
        _size(size), _renderer(renderer)
    {
        createNativeView();
    }
    
    View::~View()
    {
    }
    
#ifndef OUZEL_PLATFORM_OSX
    void View::lock()
    {
        
    }
#endif
    
    void View::resize(const Size2& size)
    {
        _size = size;
        _renderer->recalculateProjection();
    }
}
