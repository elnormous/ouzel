// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RenderTarget.h"

namespace ouzel
{
    namespace graphics
    {
        RenderTarget::RenderTarget()
        {

        }

        RenderTarget::~RenderTarget()
        {

        }

        void RenderTarget::free()
        {
            texture.reset();
        }

        bool RenderTarget::init(const Size2& newSize, bool useDepthBuffer)
        {
            size = newSize;
            depthBuffer = useDepthBuffer;

            return true;
        }
    } // namespace graphics
} // namespace ouzel
