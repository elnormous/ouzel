// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RenderTargetMetal.h"
#include "TextureMetal.h"

namespace ouzel
{
    namespace graphics
    {
        RenderTargetMetal::RenderTargetMetal()
        {

        }

        RenderTargetMetal::~RenderTargetMetal()
        {

        }

        bool RenderTargetMetal::init(const Size2& newSize, bool depthBuffer)
        {
            if (RenderTarget::init(newSize, depthBuffer))
            {
                return false;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
