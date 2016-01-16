// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RenderTarget.h"

namespace ouzel
{
    RenderTarget::RenderTarget()
    {
        
    }
    
    RenderTarget::~RenderTarget()
    {
        
    }
    
    bool RenderTarget::init(Size2 const& size, bool depthBuffer)
    {
        _size = size;
        _depthBuffer = depthBuffer;
        
        return true;
    }
}
