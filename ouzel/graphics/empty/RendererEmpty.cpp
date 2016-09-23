// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RendererEmpty.h"
#include "core/Engine.h"
#include "core/Cache.h"
#include "BlendStateEmpty.h"
#include "TextureEmpty.h"
#include "RenderTargetEmpty.h"
#include "ShaderEmpty.h"
#include "MeshBufferEmpty.h"
#include "IndexBufferEmpty.h"
#include "VertexBufferEmpty.h"

namespace ouzel
{
    namespace graphics
    {
        RendererEmpty::RendererEmpty():
            Renderer(Driver::EMPTY)
        {
        }

        RendererEmpty::~RendererEmpty()
        {
        }

        bool RendererEmpty::init(const WindowPtr& window,
                                 uint32_t newSampleCount,
                                 TextureFilter newTextureFilter,
                                 PixelFormat newBackBufferFormat,
                                 bool newVerticalSync)
        {
            if (!Renderer::init(window, newSampleCount, newTextureFilter, newBackBufferFormat, newVerticalSync))
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

        BlendStatePtr RendererEmpty::createBlendState()
        {
            BlendStatePtr blendState(new BlendStateEmpty());
            return blendState;
        }

        TexturePtr RendererEmpty::createTexture()
        {
            TexturePtr texture(new TextureEmpty());
            return texture;
        }

        RenderTargetPtr RendererEmpty::createRenderTarget()
        {
            RenderTargetPtr renderTarget(new RenderTargetEmpty());
            return renderTarget;
        }

        ShaderPtr RendererEmpty::createShader()
        {
            ShaderPtr shader(new ShaderEmpty());
            return shader;
        }

        MeshBufferPtr RendererEmpty::createMeshBuffer()
        {
            MeshBufferPtr meshBuffer(new MeshBufferEmpty());
            return meshBuffer;
        }

        IndexBufferPtr RendererEmpty::createIndexBuffer()
        {
            IndexBufferPtr indexBuffer(new IndexBufferEmpty());
            return indexBuffer;
        }

        VertexBufferPtr RendererEmpty::createVertexBuffer()
        {
            VertexBufferPtr vertexBuffer(new VertexBufferEmpty());
            return vertexBuffer;
        }
    } // namespace graphics
} // namespace ouzel
