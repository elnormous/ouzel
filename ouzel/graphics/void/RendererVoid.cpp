// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RendererVoid.h"
#include "core/Engine.h"
#include "core/Cache.h"
#include "BlendStateVoid.h"
#include "TextureVoid.h"
#include "RenderTargetVoid.h"
#include "ShaderVoid.h"
#include "MeshBufferVoid.h"
#include "IndexBufferVoid.h"
#include "VertexBufferVoid.h"

namespace ouzel
{
    namespace graphics
    {
        RendererVoid::RendererVoid():
            Renderer(Driver::VOID)
        {
        }

        RendererVoid::~RendererVoid()
        {
        }

        bool RendererVoid::init(const WindowPtr& window,
                                uint32_t newSampleCount,
                                TextureFiltering newTextureFiltering,
                                bool newVerticalSync)
        {
            if (!Renderer::init(window, newSampleCount, newTextureFiltering, newVerticalSync))
            {
                return false;
            }

            ShaderPtr textureShader = createShader();

            textureShader->initFromBuffers({ },
                                           { },
                                           VertexPCT::ATTRIBUTES,
                                           {{"color", 4 * sizeof(float)}},
                                           {{"modelViewProj", sizeof(Matrix4)}});

            sharedEngine->getCache()->setShader(SHADER_TEXTURE, textureShader);

            ShaderPtr colorShader = createShader();

            colorShader->initFromBuffers({ },
                                         { },
                                         VertexPC::ATTRIBUTES,
                                         {{"color", 4 * sizeof(float)}},
                                         {{"modelViewProj", sizeof(Matrix4)}});

            sharedEngine->getCache()->setShader(SHADER_COLOR, colorShader);

            BlendStatePtr noBlendState = createBlendState();

            noBlendState->init(false,
                               BlendState::BlendFactor::ONE, BlendState::BlendFactor::ZERO,
                               BlendState::BlendOperation::ADD,
                               BlendState::BlendFactor::ONE, BlendState::BlendFactor::ZERO,
                               BlendState::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_NO_BLEND, noBlendState);

            BlendStatePtr addBlendState = createBlendState();

            addBlendState->init(true,
                                BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                BlendState::BlendOperation::ADD,
                                BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                BlendState::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_ADD, addBlendState);

            BlendStatePtr multiplyBlendState = createBlendState();

            multiplyBlendState->init(true,
                                     BlendState::BlendFactor::DEST_COLOR, BlendState::BlendFactor::ZERO,
                                     BlendState::BlendOperation::ADD,
                                     BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                     BlendState::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_MULTIPLY, multiplyBlendState);

            BlendStatePtr alphaBlendState = createBlendState();

            alphaBlendState->init(true,
                                  BlendState::BlendFactor::SRC_ALPHA, BlendState::BlendFactor::INV_SRC_ALPHA,
                                  BlendState::BlendOperation::ADD,
                                  BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                  BlendState::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_ALPHA, alphaBlendState);

            TexturePtr whitePixelTexture = createTexture();
            whitePixelTexture->initFromBuffer( { 255, 255, 255, 255 }, Size2(1.0f, 1.0f), false, false);
            sharedEngine->getCache()->setTexture(TEXTURE_WHITE_PIXEL, whitePixelTexture);

            return true;
        }

        BlendStatePtr RendererVoid::createBlendState()
        {
            BlendStatePtr blendState(new BlendStateVoid());
            return blendState;
        }

        TexturePtr RendererVoid::createTexture()
        {
            TexturePtr texture(new TextureVoid());
            return texture;
        }

        RenderTargetPtr RendererVoid::createRenderTarget()
        {
            RenderTargetPtr renderTarget(new RenderTargetVoid());
            return renderTarget;
        }

        ShaderPtr RendererVoid::createShader()
        {
            ShaderPtr shader(new ShaderVoid());
            return shader;
        }

        MeshBufferPtr RendererVoid::createMeshBuffer()
        {
            MeshBufferPtr meshBuffer(new MeshBufferVoid());
            return meshBuffer;
        }

        IndexBufferPtr RendererVoid::createIndexBuffer()
        {
            IndexBufferPtr indexBuffer(new IndexBufferVoid());
            return indexBuffer;
        }

        VertexBufferPtr RendererVoid::createVertexBuffer()
        {
            VertexBufferPtr vertexBuffer(new VertexBufferVoid());
            return vertexBuffer;
        }
    } // namespace graphics
} // namespace ouzel
