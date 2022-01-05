// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_METALRENDERTARGET_HPP
#define OUZEL_GRAPHICS_METALRENDERTARGET_HPP

#include "../../core/Setup.h"

#if OUZEL_COMPILE_METAL

#ifdef __OBJC__
#  import <Metal/Metal.h>
using MTLRenderPassDescriptorPtr = MTLRenderPassDescriptor*;
#else
#  include <objc/objc.h>
using MTLRenderPassDescriptorPtr = id;
using MTLLoadAction = NSUInteger;
#endif

#include <set>
#include "MetalRenderResource.hpp"
#include "../../math/Color.hpp"
#include "../../platform/objc/Pointer.hpp"

namespace ouzel::graphics::metal
{
    class RenderDevice;
    class Texture;

    class RenderTarget final: public RenderResource
    {
    public:
        RenderTarget(RenderDevice& initRenderDevice,
                     const std::set<Texture*>& initColorTextures,
                     Texture* initDepthTexture);

        auto& getColorTextures() const noexcept { return colorTextures; }
        auto getDepthTexture() const noexcept { return depthTexture; }

        auto getSampleCount() const noexcept { return sampleCount; }
        auto& getColorFormats() const noexcept { return colorFormats; }
        auto getDepthFormat() const noexcept { return depthFormat; }
        auto getStencilFormat() const noexcept { return stencilFormat; }

        auto& getRenderPassDescriptor() const noexcept { return renderPassDescriptor; }

    private:
        std::set<Texture*> colorTextures;
        Texture* depthTexture = nullptr;

        NSUInteger sampleCount = 0;
        std::vector<MTLPixelFormat> colorFormats;
        MTLPixelFormat depthFormat;
        MTLPixelFormat stencilFormat;

        platform::objc::Pointer<MTLRenderPassDescriptorPtr> renderPassDescriptor;
    };
}

#endif

#endif // OUZEL_GRAPHICS_METALRENDERTARGET_HPP
