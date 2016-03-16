// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RenderTargetMetal.h"
#include "TextureMetal.h"

namespace ouzel
{
    namespace video
    {
        RenderTargetMetal::RenderTargetMetal()
        {
            
        }
        
        RenderTargetMetal::~RenderTargetMetal()
        {
            
        }
        
        bool RenderTargetMetal::init(const Size2& size, bool depthBuffer)
        {
            if (RenderTarget::init(size, depthBuffer))
            {
                return false;
            }
            
            return true;
        }
    } // namespace video
} // namespace ouzel
