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
        MetalRenderTarget::MetalRenderTarget(MetalRenderDevice& renderDeviceMetal,
                                             const std::set<MetalTexture*>& initColorTextures,
                                             MetalTexture* initDepthTexture):
            MetalRenderResource(renderDeviceMetal),
            colorTextures(initColorTextures),
            depthTexture(initDepthTexture),
            depthFormat(MTLPixelFormatInvalid),
            stencilFormat(MTLPixelFormatInvalid)
        {
            renderPassDescriptor = [[MTLRenderPassDescriptor renderPassDescriptor] retain];

            if (!renderPassDescriptor)
                throw std::runtime_error("Failed to create Metal render pass descriptor");

            for (MetalTexture* colorTexture : colorTextures)
            {
                if (colorTexture)
                {
                    size_t index = colorTextures.size() - 1;
                    renderPassDescriptor.colorAttachments[index].storeAction = (colorTexture->getSampleCount() > 1) ? MTLStoreActionMultisampleResolve : MTLStoreActionStore;
                    renderPassDescriptor.colorAttachments[index].texture = colorTexture->getTexture();

                    colorFormats.push_back(colorTexture->getPixelFormat());

                    sampleCount = colorTexture->getSampleCount();
                }
            }

            if (depthTexture)
            {
                renderPassDescriptor.depthAttachment.storeAction = (depthTexture->getSampleCount() > 1) ? MTLStoreActionMultisampleResolve : MTLStoreActionStore;
                renderPassDescriptor.depthAttachment.texture = depthTexture->getTexture();
                depthFormat = depthTexture->getPixelFormat();
                stencilFormat = depthTexture->getStencilBuffer() ? depthTexture->getPixelFormat() : MTLPixelFormatInvalid;
                sampleCount = depthTexture->getSampleCount();
            }
            else
                depthFormat = MTLPixelFormatInvalid;
        }

        MetalRenderTarget::~MetalRenderTarget()
        {
            if (renderPassDescriptor)
                [renderPassDescriptor release];
        }
    } // namespace graphics
} // namespace ouzel

#endif
