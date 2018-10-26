// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "graphics/RenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceEmpty final: public RenderDevice
        {
            friend Renderer;
        protected:
            RenderDeviceEmpty();

            virtual void init(Window* newWindow,
                              const Size2& newSize,
                              uint32_t newSampleCount,
                              Texture::Filter newTextureFilter,
                              uint32_t newMaxAnisotropy,
                              bool newVerticalSync,
                              bool newDepth,
                              bool newDebugRenderer) override;

            virtual void process() override;

            virtual RenderResource* createBlendState() override;
            virtual RenderResource* createBuffer() override;
            virtual RenderResource* createRenderTarget() override;
            virtual RenderResource* createTexture() override;
            virtual RenderResource* createShader() override;
        };
    } // namespace graphics
} // namespace ouzel
