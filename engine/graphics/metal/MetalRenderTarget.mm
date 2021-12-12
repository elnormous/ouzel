// Ouzel by Elviss Strazdins

#include "../../core/Setup.h"

#if OUZEL_COMPILE_METAL

#include "MetalRenderTarget.hpp"
#include "MetalError.hpp"
#include "MetalRenderDevice.hpp"
#include "MetalTexture.hpp"

namespace ouzel::graphics::metal
{
    RenderTarget::RenderTarget(RenderDevice& initRenderDevice,
                               const std::set<Texture*>& initColorTextures,
                               Texture* initDepthTexture):
        RenderResource{initRenderDevice},
        colorTextures{initColorTextures},
        depthTexture{initDepthTexture},
        depthFormat{MTLPixelFormatInvalid},
        stencilFormat{MTLPixelFormatInvalid}
    {
        renderPassDescriptor = [[MTLRenderPassDescriptor renderPassDescriptor] retain];

        if (!renderPassDescriptor)
            throw Error{"Failed to create Metal render pass descriptor"};

        for (const auto colorTexture : colorTextures)
            if (colorTexture)
            {
                std::size_t index = colorTextures.size() - 1;
                renderPassDescriptor.get().colorAttachments[index].storeAction = (colorTexture->getSampleCount() > 1) ? MTLStoreActionMultisampleResolve : MTLStoreActionStore;
                renderPassDescriptor.get().colorAttachments[index].texture = colorTexture->getTexture().get();

                colorFormats.push_back(colorTexture->getPixelFormat());

                sampleCount = colorTexture->getSampleCount();
            }

        if (depthTexture)
        {
            renderPassDescriptor.get().depthAttachment.storeAction = (depthTexture->getSampleCount() > 1) ? MTLStoreActionMultisampleResolve : MTLStoreActionStore;
            renderPassDescriptor.get().depthAttachment.texture = depthTexture->getTexture().get();
            depthFormat = depthTexture->getPixelFormat();
            stencilFormat = depthTexture->getStencilBuffer() ? depthTexture->getPixelFormat() : MTLPixelFormatInvalid;
            sampleCount = depthTexture->getSampleCount();
        }
        else
            depthFormat = MTLPixelFormatInvalid;
    }
}

#endif
