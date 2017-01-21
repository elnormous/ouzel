// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RendererEmpty.h"
#include "core/Engine.h"
#include "core/Cache.h"
#include "BlendStateEmpty.h"
#include "TextureEmpty.h"
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

        bool RendererEmpty::init(Window* newWindow,
                                 const Size2& newSize,
                                 uint32_t newSampleCount,
                                 Texture::Filter newTextureFilter,
                                 PixelFormat newBackBufferFormat,
                                 bool newVerticalSync,
                                 bool newDepth)
        {
            if (!Renderer::init(newWindow, newSize, newSampleCount, newTextureFilter, newBackBufferFormat, newVerticalSync, newDepth))
            {
                return false;
            }

            ShaderResourcePtr textureShader = createShader();

            textureShader->initFromBuffers({},
                                           {},
                                           VertexPCT::ATTRIBUTES,
                                           {{"color", Shader::DataType::FLOAT_VECTOR4}},
                                           {{"modelViewProj", Shader::DataType::FLOAT_MATRIX4}});

            sharedEngine->getCache()->setShader(SHADER_TEXTURE, textureShader);

            ShaderResourcePtr colorShader = createShader();

            colorShader->initFromBuffers({},
                                         {},
                                         VertexPC::ATTRIBUTES,
                                         {{"color", Shader::DataType::FLOAT_VECTOR4}},
                                         {{"modelViewProj", Shader::DataType::FLOAT_MATRIX4}});

            sharedEngine->getCache()->setShader(SHADER_COLOR, colorShader);

            BlendStateResourcePtr noBlendState = createBlendState();

            noBlendState->init(false,
                               BlendState::BlendFactor::ONE, BlendState::BlendFactor::ZERO,
                               BlendState::BlendOperation::ADD,
                               BlendState::BlendFactor::ONE, BlendState::BlendFactor::ZERO,
                               BlendState::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_NO_BLEND, noBlendState);

            BlendStateResourcePtr addBlendState = createBlendState();

            addBlendState->init(true,
                                BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                BlendState::BlendOperation::ADD,
                                BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                BlendState::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_ADD, addBlendState);

            BlendStateResourcePtr multiplyBlendState = createBlendState();

            multiplyBlendState->init(true,
                                     BlendState::BlendFactor::DEST_COLOR, BlendState::BlendFactor::ZERO,
                                     BlendState::BlendOperation::ADD,
                                     BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                     BlendState::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_MULTIPLY, multiplyBlendState);

            BlendStateResourcePtr alphaBlendState = createBlendState();

            alphaBlendState->init(true,
                                  BlendState::BlendFactor::SRC_ALPHA, BlendState::BlendFactor::INV_SRC_ALPHA,
                                  BlendState::BlendOperation::ADD,
                                  BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                  BlendState::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_ALPHA, alphaBlendState);

            TextureResourcePtr whitePixelTexture = createTexture();
            whitePixelTexture->initFromBuffer({255, 255, 255, 255}, Size2(1.0f, 1.0f), false, false);
            sharedEngine->getCache()->setTexture(TEXTURE_WHITE_PIXEL, whitePixelTexture);

            return true;
        }

        BlendStateResourcePtr RendererEmpty::createBlendState()
        {
            BlendStateResourcePtr blendState = std::make_shared<BlendStateEmpty>();
            return blendState;
        }

        TextureResourcePtr RendererEmpty::createTexture()
        {
            TextureResourcePtr texture(new TextureEmpty());
            return texture;
        }

        ShaderResourcePtr RendererEmpty::createShader()
        {
            ShaderResourcePtr shader = std::make_shared<ShaderEmpty>();
            return shader;
        }

        MeshBufferResourcePtr RendererEmpty::createMeshBuffer()
        {
            MeshBufferResourcePtr meshBuffer = std::make_shared<MeshBufferEmpty>();
            return meshBuffer;
        }

        IndexBufferResourcePtr RendererEmpty::createIndexBuffer()
        {
            IndexBufferResourcePtr indexBuffer = std::make_shared<IndexBufferEmpty>();
            return indexBuffer;
        }

        VertexBufferResourcePtr RendererEmpty::createVertexBuffer()
        {
            VertexBufferResourcePtr vertexBuffer = std::make_shared<VertexBufferEmpty>();
            return vertexBuffer;
        }
    } // namespace graphics
} // namespace ouzel
