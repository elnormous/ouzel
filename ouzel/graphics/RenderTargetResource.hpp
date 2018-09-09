// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "graphics/RenderResource.hpp"
#include "math/Color.hpp"
#include "math/Size2.hpp"

namespace ouzel
{
    namespace graphics
    {
        class TextureResource;

        class RenderTargetResource: public RenderResource
        {
        public:
            TextureResource* getColorTexture() const { return colorTexture; }
            TextureResource* getDepthTexture() const { return depthTexture; }

        protected:
            RenderTargetResource(RenderDevice& initRenderDevice);

            TextureResource* colorTexture = nullptr;
            TextureResource* depthTexture = nullptr;

            Size2 size;
            bool clearColorBuffer = true;
            bool clearDepthBuffer = false;
            Color clearColor;
            float clearDepth = 1.0F;
        };
    } // namespace graphics
} // namespace ouzel
