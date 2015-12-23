// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "RenderTarget.h"

namespace ouzel
{
    class RenderTargetD3D11: public RenderTarget
    {
    public:
        RenderTargetD3D11(Renderer* renderer);
        virtual ~RenderTargetD3D11();
    };
}
