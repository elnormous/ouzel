// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RenderTarget.h"

namespace ouzel
{
    namespace video
    {
        RenderTarget::RenderTarget()
        {
            
        }
        
        RenderTarget::~RenderTarget()
        {
            
        }
        
        bool RenderTarget::init(const Size2& size, bool depthBuffer)
        {
            _size = size;
            _depthBuffer = depthBuffer;
            
            return true;
        }
    } // namespace video
} // namespace ouzel
