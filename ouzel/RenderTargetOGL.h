// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "RenderTarget.h"

namespace ouzel
{
    class RenderTargetOGL: public RenderTarget
    {
        RenderTargetOGL(Renderer* renderer);
        virtual ~RenderTargetOGL();
    };
}
