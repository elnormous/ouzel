// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_METALTEXTURE_HPP
#define OUZEL_GRAPHICS_METALTEXTURE_HPP

#include <utility>

#include "../../core/Setup.h"

#if OUZEL_COMPILE_METAL

#ifdef __OBJC__
#  import <Metal/Metal.h>
typedef id<MTLSamplerState> MTLSamplerStatePtr;
typedef id<MTLTexture> MTLTexturePtr;
typedef MTLRenderPassDescriptor* MTLRenderPassDescriptorPtr;
#else
#  include <objc/objc.h>
using MTLSamplerStatePtr = id;
using MTLTexturePtr = id;
using MTLRenderPassDescriptorPtr = id;
using MTLPixelFormat = NSUInteger;
using MTLLoadAction = NSUInteger;
#endif

#include <tuple>
#include "MetalRenderResource.hpp"
#include "../Flags.hpp"
#include "../PixelFormat.hpp"
#include "../SamplerAddressMode.hpp"
#include "../SamplerFilter.hpp"
#include "../TextureType.hpp"
#include "../../math/Size.hpp"
#include "../../platform/objc/Pointer.hpp"

namespace ouzel::graphics::metal
{
    class RenderDevice;

    class SamplerStateDescriptor final
    {
    public:
        SamplerFilter filter;
        SamplerAddressMode addressX;
        SamplerAddressMode addressY;
        SamplerAddressMode addressZ;
        std::uint32_t maxAnisotropy;

        bool operator<(const SamplerStateDescriptor& other) const noexcept
        {
            return std::tie(filter, addressX, addressY, addressZ, maxAnisotropy) <
                std::tie(other.filter, other.addressX, other.addressY, other.addressZ, other.maxAnisotropy);
        }
    };

    class Texture final: public RenderResource
    {
    public:
        Texture(RenderDevice& initRenderDevice,
                const std::vector<std::pair<math::Size<std::uint32_t, 2>, std::vector<std::uint8_t>>>& levels,
                TextureType type,
                Flags initFlags,
                std::uint32_t initSampleCount,
                PixelFormat initPixelFormat,
                SamplerFilter initFilter,
                std::uint32_t initMaxAnisotropy);

        void setData(const std::vector<std::pair<math::Size<std::uint32_t, 2>, std::vector<std::uint8_t>>>& levels);
        void setFilter(SamplerFilter filter);
        void setAddressX(SamplerAddressMode addressX);
        void setAddressY(SamplerAddressMode addressY);
        void setAddressZ(SamplerAddressMode addressZ);
        void setMaxAnisotropy(std::uint32_t maxAnisotropy);

        auto getFlags() const noexcept { return flags; }
        auto getMipmaps() const noexcept { return mipmaps; }
        auto getSampleCount() const noexcept { return sampleCount; }

        auto& getTexture() const noexcept { return texture; }
        auto getPixelFormat() const noexcept { return pixelFormat; }
        auto getStencilBuffer() const noexcept { return stencilBuffer; }

        auto getSamplerState() const noexcept { return samplerState; }

        auto getWidth() const noexcept { return width; }
        auto getHeight() const noexcept { return height; }

    private:
        void updateSamplerState();

        Flags flags = Flags::none;
        std::uint32_t mipmaps = 0;
        std::uint32_t sampleCount = 1;

        SamplerStateDescriptor samplerDescriptor;

        platform::objc::Pointer<MTLTexturePtr> texture;

        NSUInteger width = 0;
        NSUInteger height = 0;

        MTLSamplerStatePtr samplerState = nil;
        platform::objc::Pointer<MTLTexturePtr> msaaTexture;

        MTLPixelFormat pixelFormat;
        std::uint32_t pixelSize = 0;
        bool stencilBuffer = false;
    };
}

#endif

#endif // OUZEL_GRAPHICS_METALTEXTURE_HPP
