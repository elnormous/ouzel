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
            resource(initRenderer)
        {
            initRenderer.addCommand(std::unique_ptr<Command>(new InitRenderTargetCommand(resource.getId())));
        }

        void RenderTarget::addColorTexture(const std::shared_ptr<Texture>& texture)
        {
            auto i = std::find_if(colorTextures.begin(), colorTextures.end(), [&texture](const std::shared_ptr<Texture>& other) {
                return texture.get() == other.get();
            });

            if (i == colorTextures.end())
            {
                colorTextures.push_back(texture);

                if (resource.getId())
                    resource.getRenderer()->addCommand(std::unique_ptr<Command>(new AddRenderTargetColorTextureCommand(resource.getId(),
                                                                                                                       texture->getResource())));
            }
        }

        void RenderTarget::removeColorTexture(const std::shared_ptr<Texture>& texture)
        {
            auto i = std::find_if(colorTextures.begin(), colorTextures.end(), [&texture](const std::shared_ptr<Texture>& other) {
                return texture.get() == other.get();
            });

            if (i != colorTextures.end())
            {
                colorTextures.erase(i);

                if (resource.getId())
                    resource.getRenderer()->addCommand(std::unique_ptr<Command>(new RemoveRenderTargetColorTextureCommand(resource.getId(),
                                                                                                                          texture->getResource())));
            }
        }

        void RenderTarget::removeColorTexture(Texture* texture)
        {
            auto i = std::find_if(colorTextures.begin(), colorTextures.end(), [texture](const std::shared_ptr<Texture>& other) {
                return texture == other.get();
            });

            if (i != colorTextures.end())
            {
                colorTextures.erase(i);

                if (resource.getId())
                    resource.getRenderer()->addCommand(std::unique_ptr<Command>(new RemoveRenderTargetColorTextureCommand(resource.getId(),
                                                                                                                          texture->getResource())));
            }
        }

        void RenderTarget::setDepthTexture(const std::shared_ptr<Texture>& texture)
        {
            depthTexture = texture;

            if (resource.getId())
                resource.getRenderer()->addCommand(std::unique_ptr<Command>(new SetRenderTargetDepthTextureCommand(resource.getId(),
                                                                                                                   texture ? texture->getResource() : 0)));
        }
    } // namespace graphics
} // namespace ouzel
