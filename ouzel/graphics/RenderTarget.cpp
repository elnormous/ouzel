// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include "RenderTarget.hpp"
#include "Renderer.hpp"
#include "RenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        RenderTarget::RenderTarget(Renderer& initRenderer):
            renderer(initRenderer),
            resource(renderer.getDevice()->getResourceId())
        {
        }

        RenderTarget::~RenderTarget()
        {
            if (resource)
            {
                renderer.addCommand(std::unique_ptr<Command>(new DeleteResourceCommand(resource)));
                RenderDevice* renderDevice = renderer.getDevice();
                renderDevice->deleteResourceId(resource);
            }
        }

        void RenderTarget::addColorTexture(const std::shared_ptr<Texture>& texture)
        {
            auto i = std::find_if(colorTextures.begin(), colorTextures.end(), [&texture](const std::shared_ptr<Texture>& other) {
                return texture.get() == other.get();
            });

            if (i == colorTextures.end())
                colorTextures.push_back(texture);
        }

        void RenderTarget::removeColorTexture(const std::shared_ptr<Texture>& texture)
        {
            auto i = std::find_if(colorTextures.begin(), colorTextures.end(), [&texture](const std::shared_ptr<Texture>& other) {
                return texture.get() == other.get();
            });

            if (i != colorTextures.end())
                colorTextures.erase(i);
        }

        void RenderTarget::removeColorTexture(Texture* texture)
        {
            auto i = std::find_if(colorTextures.begin(), colorTextures.end(), [texture](const std::shared_ptr<Texture>& other) {
                return texture == other.get();
            });

            if (i != colorTextures.end())
                colorTextures.erase(i);
        }

        void RenderTarget::setDepthTexture(const std::shared_ptr<Texture>& texture)
        {
            depthTexture = texture;
        }

        void RenderTarget::setClearColorBuffer(bool clear)
        {
            clearColorBuffer = clear;
        }

        void RenderTarget::setClearDepthBuffer(bool clear)
        {
            clearDepthBuffer = clear;
        }

        void RenderTarget::setClearColor(Color color)
        {
            clearColor = color;
        }

        void RenderTarget::setClearDepth(float clear)
        {
            clearDepth = clear;
        }
    } // namespace graphics
} // namespace ouzel
