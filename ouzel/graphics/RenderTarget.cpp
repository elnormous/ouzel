// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "RenderTarget.hpp"
#include "Renderer.hpp"
#include "RenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        RenderTarget::RenderTarget(Renderer& initRenderer):
            renderer(initRenderer)
        {
            resource = renderer.getDevice()->getResourceId();
        }

        RenderTarget::~RenderTarget()
        {
            if (resource)
            {
                RenderDevice* renderDevice = renderer.getDevice();
                renderDevice->addCommand(std::unique_ptr<Command>(new DeleteRenderTargetCommand(resource)));
                renderDevice->deleteResourceId(resource);
            }
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
