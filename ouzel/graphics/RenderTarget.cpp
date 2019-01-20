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
            renderer.addCommand(std::unique_ptr<Command>(new InitRenderTargetCommand(resource,
                                                                                clearColorBuffer,
                                                                                clearDepthBuffer,
                                                                                clearColor,
                                                                                clearDepth)));
        }

        RenderTarget::RenderTarget(Renderer& initRenderer,
                                   bool initClearColorBuffer,
                                   bool initClearDepthBuffer,
                                   Color initClearColor,
                                   float initClearDepth):
            renderer(initRenderer),
            clearColorBuffer(initClearColorBuffer),
            clearDepthBuffer(initClearDepthBuffer),
            clearColor(initClearColor),
            clearDepth(initClearDepth)
        {
            renderer.addCommand(std::unique_ptr<Command>(new InitRenderTargetCommand(resource,
                                                                                     clearColorBuffer,
                                                                                     clearDepthBuffer,
                                                                                     clearColor,
                                                                                     clearDepth)));
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
            {
                colorTextures.push_back(texture);

                renderer.addCommand(std::unique_ptr<Command>(new AddRenderTargetColorTextureCommand(resource,
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

                renderer.addCommand(std::unique_ptr<Command>(new RemoveRenderTargetColorTextureCommand(resource,
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

                renderer.addCommand(std::unique_ptr<Command>(new RemoveRenderTargetColorTextureCommand(resource,
                                                                                                       texture->getResource())));
            }
        }

        void RenderTarget::setDepthTexture(const std::shared_ptr<Texture>& texture)
        {
            depthTexture = texture;

            renderer.addCommand(std::unique_ptr<Command>(new SetRenderTargetParametersCommand(resource,
                                                                                              clearColorBuffer,
                                                                                              clearDepthBuffer,
                                                                                              clearColor,
                                                                                              clearDepth)));

            renderer.addCommand(std::unique_ptr<Command>(new SetRenderTargetDepthTextureCommand(resource,
                                                                                                texture ? texture->getResource() : 0)));
        }

        void RenderTarget::setClearColorBuffer(bool clear)
        {
            clearColorBuffer = clear;

            renderer.addCommand(std::unique_ptr<Command>(new SetRenderTargetParametersCommand(resource,
                                                                                              clearColorBuffer,
                                                                                              clearDepthBuffer,
                                                                                              clearColor,
                                                                                              clearDepth)));
        }

        void RenderTarget::setClearDepthBuffer(bool clear)
        {
            clearDepthBuffer = clear;

            renderer.addCommand(std::unique_ptr<Command>(new SetRenderTargetParametersCommand(resource,
                                                                                              clearColorBuffer,
                                                                                              clearDepthBuffer,
                                                                                              clearColor,
                                                                                              clearDepth)));
        }

        void RenderTarget::setClearColor(Color color)
        {
            clearColor = color;

            renderer.addCommand(std::unique_ptr<Command>(new SetRenderTargetParametersCommand(resource,
                                                                                              clearColorBuffer,
                                                                                              clearDepthBuffer,
                                                                                              clearColor,
                                                                                              clearDepth)));
        }

        void RenderTarget::setClearDepth(float clear)
        {
            clearDepth = clear;

            renderer.addCommand(std::unique_ptr<Command>(new SetRenderTargetParametersCommand(resource,
                                                                                              clearColorBuffer,
                                                                                              clearDepthBuffer,
                                                                                              clearColor,
                                                                                              clearDepth)));
        }
    } // namespace graphics
} // namespace ouzel
