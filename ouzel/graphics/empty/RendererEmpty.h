// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "graphics/Renderer.h"

namespace ouzel
{
    class Engine;

    namespace graphics
    {
        class RendererEmpty: public Renderer
        {
            friend Engine;
        public:
            virtual BlendStateResourcePtr createBlendState() override;
            virtual TextureResourcePtr createTexture() override;
            virtual ShaderResourcePtr createShader() override;
            virtual MeshBufferResourcePtr createMeshBuffer() override;
            virtual IndexBufferResourcePtr createIndexBuffer() override;
            virtual VertexBufferResourcePtr createVertexBuffer() override;

        protected:
            RendererEmpty();

            virtual bool init(Window* newWindow,
                              const Size2& newSize,
                              uint32_t newSampleCount,
                              TextureResource::Filter newTextureFilter,
                              PixelFormat newBackBufferFormat,
                              bool newVerticalSync,
                              bool newDepth) override;
        };
    } // namespace graphics
} // namespace ouzel
