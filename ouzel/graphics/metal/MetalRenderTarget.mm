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
            depthFormat(MTLPixelFormatInvalid)
        {
            renderPassDescriptor = [[MTLRenderPassDescriptor renderPassDescriptor] retain];

            if (!renderPassDescriptor)
                throw std::runtime_error("Failed to create Metal render pass descriptor");
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

                colorFormats.push_back(texture->getPixelFormat());

                sampleCount = texture->getSampleCount();
            }
        }

        void MetalRenderTarget::removeColorTexture(MetalTexture* texture)
        {
            auto i = colorTextures.find(texture);

            if (i != colorTextures.end())
            {
                colorTextures.erase(i);

                colorFormats.clear();

                size_t index = 0;
                for (MetalTexture* colorTexture : colorTextures)
                {
                    renderPassDescriptor.colorAttachments[index].storeAction = (colorTexture->getSampleCount() > 1) ? MTLStoreActionMultisampleResolve : MTLStoreActionStore;
                    renderPassDescriptor.colorAttachments[index].texture = colorTexture->getTexture();

                    colorFormats.push_back(colorTexture->getPixelFormat());

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
                depthFormat = texture->getPixelFormat();
                sampleCount = texture->getSampleCount();
            }
            else
                depthFormat = MTLPixelFormatInvalid;
        }
    } // namespace graphics
} // namespace ouzel

#endif
