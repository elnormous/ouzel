// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#pragma once

#include "graphics/RenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceEmpty: public RenderDevice
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

            virtual void processCommands(CommandBuffer& commands) override;

            virtual BlendStateResource* createBlendState() override;
            virtual BufferResource* createBuffer() override;
            virtual RenderTargetResource* createRenderTarget() override;
            virtual TextureResource* createTexture() override;
            virtual ShaderResource* createShader() override;
        };
    } // namespace graphics
} // namespace ouzel
