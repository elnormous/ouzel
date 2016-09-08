// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "graphics/Renderer.h"

namespace ouzel
{
    class Engine;

    namespace graphics
    {
        class RendererVoid: public Renderer
        {
            friend Engine;
            friend Window;
        public:
            virtual ~RendererVoid();

            virtual BlendStatePtr createBlendState() override;
            virtual TexturePtr createTexture() override;
            virtual RenderTargetPtr createRenderTarget() override;
            virtual ShaderPtr createShader() override;
            virtual MeshBufferPtr createMeshBuffer() override;
            virtual IndexBufferPtr createIndexBuffer() override;
            virtual VertexBufferPtr createVertexBuffer() override;

        protected:
            RendererVoid();
            
            virtual bool init(const WindowPtr& window,
                              uint32_t newSampleCount,
                              TextureFiltering newTextureFiltering,
                              bool newVerticalSync) override;
        };
    } // namespace graphics
} // namespace ouzel
