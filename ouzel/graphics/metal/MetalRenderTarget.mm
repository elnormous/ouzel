// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

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
        namespace metal
        {
            RenderTarget::RenderTarget(RenderDevice& renderDevice,
                                       const std::set<Texture*>& initColorTextures,
                                       Texture* initDepthTexture):
                RenderResource(renderDevice),
                colorTextures(initColorTextures),
                depthTexture(initDepthTexture),
                depthFormat(MTLPixelFormatInvalid),
                stencilFormat(MTLPixelFormatInvalid)
            {
                renderPassDescriptor = [[MTLRenderPassDescriptor renderPassDescriptor] retain];

                if (!renderPassDescriptor)
                    throw std::runtime_error("Failed to create Metal render pass descriptor");

                for (Texture* colorTexture : colorTextures)
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

            RenderTarget::~RenderTarget()
            {
                if (renderPassDescriptor)
                    [renderPassDescriptor release];
            }
        } // namespace metal
    } // namespace graphics
} // namespace ouzel

#endif
