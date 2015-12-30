// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "RenderTarget.h"

namespace ouzel
{
    class RendererOGL;
    
    class RenderTargetOGL: public RenderTarget
    {
        friend RendererOGL;
    public:
        virtual ~RenderTargetOGL();
        
    protected:
        RenderTargetOGL();
    };
}
