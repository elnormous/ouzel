// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "View.h"
#include "OpenGLView.h"

namespace ouzel
{
    void View::lock()
    {
        OpenGLView* openGLView = (OpenGLView*)_nativeView;
        
        [openGLView makeOpenGLContextCurrent];
    }
    
    void View::createNativeView()
    {
        _nativeView = [[OpenGLView alloc]initWithFrame:NSMakeRect(0, 0, _size.width, _size.height) view:this];
    }
    
    void View::destroyNativeView()
    {
        [_nativeView removeFromSuperview];
    }
}
