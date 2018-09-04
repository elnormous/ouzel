// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "RenderDeviceEmpty.hpp"
#include "BlendStateResourceEmpty.hpp"
#include "BufferResourceEmpty.hpp"
#include "RenderTargetResourceEmpty.hpp"
#include "ShaderResourceEmpty.hpp"
#include "TextureResourceEmpty.hpp"
#include "assets/Cache.hpp"

namespace ouzel
{
    namespace graphics
    {
        RenderDeviceEmpty::RenderDeviceEmpty():
            RenderDevice(Renderer::Driver::EMPTY)
        {
        }

        void RenderDeviceEmpty::init(Window* newWindow,
                                     const Size2& newSize,
                                     uint32_t newSampleCount,
                                     Texture::Filter newTextureFilter,
                                     uint32_t newMaxAnisotropy,
                                     bool newVerticalSync,
                                     bool newDepth,
                                     bool newDebugRenderer)
        {
            RenderDevice::init(newWindow,
                               newSize,
                               newSampleCount,
                               newTextureFilter,
                               newMaxAnisotropy,
                               newVerticalSync,
                               newDepth,
                               newDebugRenderer);
        }

        void RenderDeviceEmpty::process()
        {
        }

        BlendStateResource* RenderDeviceEmpty::createBlendState()
        {
            std::unique_lock<std::mutex> lock(resourceMutex);

            BlendStateResource* blendState = new BlendStateResourceEmpty(*this);
            resources.push_back(std::unique_ptr<RenderResource>(blendState));
            return blendState;
        }

        BufferResource* RenderDeviceEmpty::createBuffer()
        {
            std::unique_lock<std::mutex> lock(resourceMutex);

            BufferResource* buffer = new BufferResourceEmpty(*this);
            resources.push_back(std::unique_ptr<RenderResource>(buffer));
            return buffer;
        }

        RenderTargetResource* RenderDeviceEmpty::createRenderTarget()
        {
            std::unique_lock<std::mutex> lock(resourceMutex);

            RenderTargetResource* renderTarget = new RenderTargetResourceEmpty(*this);
            resources.push_back(std::unique_ptr<RenderResource>(renderTarget));
            return renderTarget;
        }

        ShaderResource* RenderDeviceEmpty::createShader()
        {
            std::unique_lock<std::mutex> lock(resourceMutex);

            ShaderResource* shader = new ShaderResourceEmpty(*this);
            resources.push_back(std::unique_ptr<RenderResource>(shader));
            return shader;
        }

        TextureResource* RenderDeviceEmpty::createTexture()
        {
            std::unique_lock<std::mutex> lock(resourceMutex);

            TextureResource* texture(new TextureResourceEmpty(*this));
            resources.push_back(std::unique_ptr<RenderResource>(texture));
            return texture;
        }
    } // namespace graphics
} // namespace ouzel
