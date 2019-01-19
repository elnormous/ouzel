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

        void MetalRenderTarget::setColorTexture(MetalTexture* texture)
        {
            colorTexture = texture;

            if (texture)
            {
                renderPassDescriptor.colorAttachments[0].storeAction = (texture->getSampleCount() > 1) ? MTLStoreActionMultisampleResolve : MTLStoreActionStore;
                renderPassDescriptor.colorAttachments[0].texture = texture->getTexture();
            }
        }

        void MetalRenderTarget::setDepthTexture(MetalTexture* texture)
        {
            depthTexture = texture;

            if (texture)
            {
                renderPassDescriptor.depthAttachment.storeAction = (texture->getSampleCount() > 1) ? MTLStoreActionMultisampleResolve : MTLStoreActionStore;
                renderPassDescriptor.depthAttachment.texture = texture->getTexture();
            }
        }
    } // namespace graphics
} // namespace ouzel

#endif
