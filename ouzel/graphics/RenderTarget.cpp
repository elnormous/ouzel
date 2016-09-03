// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RenderTarget.h"

namespace ouzel
{
    namespace graphics
    {
        RenderTarget::RenderTarget():
            clear(true)
        {

        }

        RenderTarget::~RenderTarget()
        {

        }

        void RenderTarget::free()
        {
            texture.reset();
            ready = false;
        }

        bool RenderTarget::init(const Size2& newSize, bool useDepthBuffer)
        {
            size = newSize;
            depthBuffer = useDepthBuffer;

            return true;
        }
    } // namespace graphics
} // namespace ouzel
