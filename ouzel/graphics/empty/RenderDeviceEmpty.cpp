// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RenderDeviceEmpty.hpp"
#include "BlendStateResourceEmpty.hpp"
#include "TextureResourceEmpty.hpp"
#include "ShaderResourceEmpty.hpp"
#include "MeshBufferResourceEmpty.hpp"
#include "BufferResourceEmpty.hpp"
#include "core/Engine.hpp"
#include "core/Cache.hpp"

namespace ouzel
{
    namespace graphics
    {
        RenderDeviceEmpty::RenderDeviceEmpty():
            RenderDevice(Renderer::Driver::EMPTY)
        {
        }

        bool RenderDeviceEmpty::init(Window* newWindow,
                                     const Size2& newSize,
                                     uint32_t newSampleCount,
                                     Texture::Filter newTextureFilter,
                                     uint32_t newMaxAnisotropy,
                                     bool newVerticalSync,
                                     bool newDepth,
                                     bool newDebugRenderer)
        {
            if (!RenderDevice::init(newWindow,
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

            std::shared_ptr<BlendState> screenBlendState = std::make_shared<BlendState>();

            screenBlendState->init(true,
                                   BlendState::BlendFactor::ONE, BlendState::BlendFactor::INV_SRC_COLOR,
                                   BlendState::BlendOperation::ADD,
                                   BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                   BlendState::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_SCREEN, screenBlendState);

            std::shared_ptr<Texture> whitePixelTexture = std::make_shared<Texture>();
            whitePixelTexture->init({255, 255, 255, 255}, Size2(1.0f, 1.0f), 0, 1);
            sharedEngine->getCache()->setTexture(TEXTURE_WHITE_PIXEL, whitePixelTexture);

            return true;
        }

        bool RenderDeviceEmpty::draw(const std::vector<DrawCommand>&)
        {
            return true;
        }

        BlendStateResource* RenderDeviceEmpty::createBlendState()
        {
            std::lock_guard<std::mutex> lock(resourceMutex);

            BlendStateResource* blendState = new BlendStateResourceEmpty();
            resources.push_back(std::unique_ptr<RenderResource>(blendState));
            return blendState;
        }

        TextureResource* RenderDeviceEmpty::createTexture()
        {
            std::lock_guard<std::mutex> lock(resourceMutex);

            TextureResource* texture(new TextureResourceEmpty());
            resources.push_back(std::unique_ptr<RenderResource>(texture));
            return texture;
        }

        ShaderResource* RenderDeviceEmpty::createShader()
        {
            std::lock_guard<std::mutex> lock(resourceMutex);

            ShaderResource* shader = new ShaderResourceEmpty();
            resources.push_back(std::unique_ptr<RenderResource>(shader));
            return shader;
        }

        MeshBufferResource* RenderDeviceEmpty::createMeshBuffer()
        {
            std::lock_guard<std::mutex> lock(resourceMutex);

            MeshBufferResource* meshBuffer = new MeshBufferResourceEmpty();
            resources.push_back(std::unique_ptr<RenderResource>(meshBuffer));
            return meshBuffer;
        }

        BufferResource* RenderDeviceEmpty::createBuffer()
        {
            std::lock_guard<std::mutex> lock(resourceMutex);

            BufferResource* buffer = new BufferResourceEmpty();
            resources.push_back(std::unique_ptr<RenderResource>(buffer));
            return buffer;
        }
    } // namespace graphics
} // namespace ouzel
