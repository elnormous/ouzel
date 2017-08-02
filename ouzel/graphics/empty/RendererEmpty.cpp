// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RendererEmpty.h"
#include "BlendStateInterfaceEmpty.h"
#include "TextureInterfaceEmpty.h"
#include "ShaderInterfaceEmpty.h"
#include "MeshBufferInterfaceEmpty.h"
#include "BufferInterfaceEmpty.h"
#include "core/Engine.h"
#include "core/Cache.h"

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
                                 uint32_t newMaxAnisotropy,
                                 bool newVerticalSync,
                                 bool newDepth,
                                 bool newDebugRenderer)
        {
            if (!Renderer::init(newWindow,
                                newSize,
                                newSampleCount,
                                newTextureFilter,
                                newMaxAnisotropy,
                                newVerticalSync,
                                newDepth,
                                newDebugRenderer))
            {
                return false;
            }

            std::shared_ptr<Shader> textureShader = std::make_shared<Shader>();

            textureShader->init(std::vector<uint8_t>(),
                                std::vector<uint8_t>(),
                                VertexPCT::ATTRIBUTES,
                                {{"color", DataType::FLOAT_VECTOR4}},
                                {{"modelViewProj", DataType::FLOAT_MATRIX4}});

            sharedEngine->getCache()->setShader(SHADER_TEXTURE, textureShader);

            std::shared_ptr<Shader> colorShader = std::make_shared<Shader>();

            colorShader->init(std::vector<uint8_t>(),
                              std::vector<uint8_t>(),
                              VertexPC::ATTRIBUTES,
                              {{"color", DataType::FLOAT_VECTOR4}},
                              {{"modelViewProj", DataType::FLOAT_MATRIX4}});

            sharedEngine->getCache()->setShader(SHADER_COLOR, colorShader);

            std::shared_ptr<BlendState> noBlendState = std::make_shared<BlendState>();

            noBlendState->init(false,
                               BlendState::BlendFactor::ONE, BlendState::BlendFactor::ZERO,
                               BlendState::BlendOperation::ADD,
                               BlendState::BlendFactor::ONE, BlendState::BlendFactor::ZERO,
                               BlendState::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_NO_BLEND, noBlendState);

            std::shared_ptr<BlendState> addBlendState = std::make_shared<BlendState>();

            addBlendState->init(true,
                                BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                BlendState::BlendOperation::ADD,
                                BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                BlendState::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_ADD, addBlendState);

            std::shared_ptr<BlendState> multiplyBlendState = std::make_shared<BlendState>();

            multiplyBlendState->init(true,
                                     BlendState::BlendFactor::DEST_COLOR, BlendState::BlendFactor::ZERO,
                                     BlendState::BlendOperation::ADD,
                                     BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                     BlendState::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_MULTIPLY, multiplyBlendState);

            std::shared_ptr<BlendState> alphaBlendState = std::make_shared<BlendState>();

            alphaBlendState->init(true,
                                  BlendState::BlendFactor::SRC_ALPHA, BlendState::BlendFactor::INV_SRC_ALPHA,
                                  BlendState::BlendOperation::ADD,
                                  BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                  BlendState::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_ALPHA, alphaBlendState);

            std::shared_ptr<Texture> whitePixelTexture = std::make_shared<Texture>();
            whitePixelTexture->init({255, 255, 255, 255}, Size2(1.0f, 1.0f), 0, 1);
            sharedEngine->getCache()->setTexture(TEXTURE_WHITE_PIXEL, whitePixelTexture);

            return true;
        }

        bool RendererEmpty::draw(const std::vector<DrawCommand>&)
        {
            return true;
        }

        BlendStateInterface* RendererEmpty::createBlendState()
        {
            std::lock_guard<std::mutex> lock(resourceMutex);

            BlendStateInterface* blendState = new BlendStateInterfaceEmpty();
            resources.push_back(std::unique_ptr<ResourceInterface>(blendState));
            return blendState;
        }

        TextureInterface* RendererEmpty::createTexture()
        {
            std::lock_guard<std::mutex> lock(resourceMutex);

            TextureInterface* texture(new TextureInterfaceEmpty());
            resources.push_back(std::unique_ptr<ResourceInterface>(texture));
            return texture;
        }

        ShaderInterface* RendererEmpty::createShader()
        {
            std::lock_guard<std::mutex> lock(resourceMutex);

            ShaderInterface* shader = new ShaderInterfaceEmpty();
            resources.push_back(std::unique_ptr<ResourceInterface>(shader));
            return shader;
        }

        MeshBufferInterface* RendererEmpty::createMeshBuffer()
        {
            std::lock_guard<std::mutex> lock(resourceMutex);

            MeshBufferInterface* meshBuffer = new MeshBufferInterfaceEmpty();
            resources.push_back(std::unique_ptr<ResourceInterface>(meshBuffer));
            return meshBuffer;
        }

        BufferInterface* RendererEmpty::createBuffer()
        {
            std::lock_guard<std::mutex> lock(resourceMutex);

            BufferInterface* buffer = new BufferInterfaceEmpty();
            resources.push_back(std::unique_ptr<ResourceInterface>(buffer));
            return buffer;
        }
    } // namespace graphics
} // namespace ouzel
