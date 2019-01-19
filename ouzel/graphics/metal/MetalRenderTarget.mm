// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

#include <stdexcept>
#include "MetalRenderTarget.hpp"
#include "MetalRenderDevice.hpp"
#include "MetalTexture.hpp"

namespace ouzel
{
    namespace graphics
    {
        MetalRenderTarget::MetalRenderTarget(MetalRenderDevice& renderDeviceMetal):
            MetalRenderResource(renderDeviceMetal),
            colorBufferLoadAction(MTLLoadActionDontCare),
            depthBufferLoadAction(MTLLoadActionDontCare)
        {
            renderPassDescriptor = [[MTLRenderPassDescriptor renderPassDescriptor] retain];

            if (!renderPassDescriptor)
                throw std::runtime_error("Failed to create Metal render pass descriptor");

            renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(0.0F, 0.0F, 0.0F, 0.0F);
            renderPassDescriptor.depthAttachment.clearDepth = 1.0F;
        }

        MetalRenderTarget::~MetalRenderTarget()
        {
            if (renderPassDescriptor)
                [renderPassDescriptor release];
        }

        void MetalRenderTarget::addColorTexture(MetalTexture* texture)
        {
            if (texture && colorTextures.insert(texture).second)
            {
                size_t index = colorTextures.size() - 1;
                renderPassDescriptor.colorAttachments[index].storeAction = (texture->getSampleCount() > 1) ? MTLStoreActionMultisampleResolve : MTLStoreActionStore;
                renderPassDescriptor.colorAttachments[index].texture = texture->getTexture();
                renderPassDescriptor.colorAttachments[index].clearColor = clearColor;
            }
        }

        void MetalRenderTarget::removeColorTexture(MetalTexture* texture)
        {
            auto i = colorTextures.find(texture);

            if (i != colorTextures.end())
            {
                colorTextures.erase(i);

                size_t index = 0;
                for (MetalTexture* colorTexture : colorTextures)
                {
                    renderPassDescriptor.colorAttachments[index].storeAction = (colorTexture->getSampleCount() > 1) ? MTLStoreActionMultisampleResolve : MTLStoreActionStore;
                    renderPassDescriptor.colorAttachments[index].texture = colorTexture->getTexture();

                    ++index;
                }

                renderPassDescriptor.colorAttachments[index].storeAction = MTLStoreActionDontCare;
                renderPassDescriptor.colorAttachments[index].texture = nil;
            }
        }

        void MetalRenderTarget::setDepthTexture(MetalTexture* texture)
        {
            depthTexture = texture;

            if (texture)
            {
                renderPassDescriptor.depthAttachment.storeAction = (texture->getSampleCount() > 1) ? MTLStoreActionMultisampleResolve : MTLStoreActionStore;
                renderPassDescriptor.depthAttachment.texture = texture->getTexture();
                renderPassDescriptor.depthAttachment.clearDepth = clearDepth;
            }
        }

        void MetalRenderTarget::setClearColorBuffer(bool clear)
        {
            colorBufferLoadAction = clear ? MTLLoadActionClear : MTLLoadActionDontCare;
        }

        void MetalRenderTarget::setClearDepthBuffer(bool clear)
        {
            depthBufferLoadAction = clear ? MTLLoadActionClear : MTLLoadActionDontCare;
        }

        void MetalRenderTarget::setClearColor(Color color)
        {
            if (!renderPassDescriptor)
                throw std::runtime_error("Render pass descriptor not initialized");

            clearColor = MTLClearColorMake(color.normR(),
                                           color.normG(),
                                           color.normB(),
                                           color.normA());

            for (size_t index = 0; index < colorTextures.size(); ++index)
            {
                renderPassDescriptor.colorAttachments[index].clearColor = clearColor;
                ++index;
            }
        }

        void MetalRenderTarget::setClearDepth(float newClearDepth)
        {
            if (!renderPassDescriptor)
                throw std::runtime_error("Render pass descriptor not initialized");

            clearDepth = newClearDepth;

            renderPassDescriptor.depthAttachment.clearDepth = clearDepth;
        }
    } // namespace graphics
} // namespace ouzel

#endif
