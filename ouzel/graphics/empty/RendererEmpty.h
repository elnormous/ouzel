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
        protected:
            RendererEmpty();

            virtual bool init(Window* newWindow,
                              const Size2& newSize,
                              uint32_t newSampleCount,
                              Texture::Filter newTextureFilter,
                              uint32_t newMaxAnisotropy,
                              bool newVerticalSync,
                              bool newDepth,
                              bool newDebugRenderer) override;

            virtual bool draw(const std::vector<DrawCommand>& drawCommands) override;

            virtual BlendStateInterface* createBlendState() override;
            virtual TextureInterface* createTexture() override;
            virtual ShaderInterface* createShader() override;
            virtual MeshBufferInterface* createMeshBuffer() override;
            virtual BufferInterface* createBuffer() override;
        };
    } // namespace graphics
} // namespace ouzel
