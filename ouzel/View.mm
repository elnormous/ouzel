// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "View.h"
#include "OpenGLView.h"

namespace ouzel
{
    void View::createNativeView()
    {
        OpenGLView* openGLView = [[OpenGLView alloc]initWithFrame:NSMakeRect(0, 0, _size.width, _size.height) view:this];
        
        _nativeView = (__bridge void*)openGLView;
    }
    
    void View::destroyNativeView()
    {
        OpenGLView* openGLView = (__bridge OpenGLView*)_nativeView;
        [openGLView removeFromSuperview];
    }
}
