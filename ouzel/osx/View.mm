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

    }
    
    void View::destroyNativeView()
    {
        [_nativeView removeFromSuperview];
    }
}
