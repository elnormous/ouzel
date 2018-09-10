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

        RenderResource* RenderDeviceEmpty::createBlendState()
        {
            std::unique_lock<std::mutex> lock(resourceMutex);

            RenderResource* blendState = new BlendStateResourceEmpty(*this);
            resources.push_back(std::unique_ptr<RenderResource>(blendState));
            return blendState;
        }

        RenderResource* RenderDeviceEmpty::createBuffer()
        {
            std::unique_lock<std::mutex> lock(resourceMutex);

            RenderResource* buffer = new BufferResourceEmpty(*this);
            resources.push_back(std::unique_ptr<RenderResource>(buffer));
            return buffer;
        }

        RenderResource* RenderDeviceEmpty::createRenderTarget()
        {
            std::unique_lock<std::mutex> lock(resourceMutex);

            RenderResource* renderTarget = new RenderTargetResourceEmpty(*this);
            resources.push_back(std::unique_ptr<RenderResource>(renderTarget));
            return renderTarget;
        }

        RenderResource* RenderDeviceEmpty::createShader()
        {
            std::unique_lock<std::mutex> lock(resourceMutex);

            RenderResource* shader = new ShaderResourceEmpty(*this);
            resources.push_back(std::unique_ptr<RenderResource>(shader));
            return shader;
        }

        RenderResource* RenderDeviceEmpty::createTexture()
        {
            std::unique_lock<std::mutex> lock(resourceMutex);

            RenderResource* texture(new TextureResourceEmpty(*this));
            resources.push_back(std::unique_ptr<RenderResource>(texture));
            return texture;
        }
    } // namespace graphics
} // namespace ouzel
