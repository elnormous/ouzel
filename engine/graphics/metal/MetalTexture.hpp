// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_METALTEXTURE_HPP
#define OUZEL_GRAPHICS_METALTEXTURE_HPP

#include <utility>

#include "../../core/Setup.h"

#if OUZEL_COMPILE_METAL

#if defined(__OBJC__)
#  import <Metal/Metal.h>
typedef id<MTLSamplerState> MTLSamplerStatePtr;
typedef id<MTLTexture> MTLTexturePtr;
typedef MTLRenderPassDescriptor* MTLRenderPassDescriptorPtr;
#else
#  include <objc/objc.h>
typedef id MTLSamplerStatePtr;
typedef id MTLTexturePtr;
typedef id MTLRenderPassDescriptorPtr;
typedef NSUInteger MTLPixelFormat;
typedef NSUInteger MTLLoadAction;
#endif

#include <tuple>
#include "MetalRenderResource.hpp"
#include "MetalPointer.hpp"
#include "../PixelFormat.hpp"
#include "../SamplerAddressMode.hpp"
#include "../SamplerFilter.hpp"
#include "../TextureType.hpp"
#include "../../math/Size.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace metal
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
                        const std::vector<std::pair<Size2U, std::vector<std::uint8_t>>>& levels,
                        TextureType type,
                        std::uint32_t initFlags = 0,
                        std::uint32_t initSampleCount = 1,
                        PixelFormat initPixelFormat = PixelFormat::RGBA8UNorm);

                void setData(const std::vector<std::pair<Size2U, std::vector<std::uint8_t>>>& levels);
                void setFilter(SamplerFilter filter);
                void setAddressX(SamplerAddressMode addressX);
                void setAddressY(SamplerAddressMode addressY);
                void setAddressZ(SamplerAddressMode addressZ);
                void setMaxAnisotropy(std::uint32_t maxAnisotropy);

                inline auto getFlags() const noexcept { return flags; }
                inline auto getMipmaps() const noexcept { return mipmaps; }
                inline auto getSampleCount() const noexcept { return sampleCount; }

                inline auto& getTexture() const noexcept { return texture; }
                inline auto getPixelFormat() const noexcept { return pixelFormat; }
                inline auto getStencilBuffer() const noexcept { return stencilBuffer; }

                inline auto getSamplerState() const noexcept { return samplerState; }

                inline auto getWidth() const noexcept { return width; }
                inline auto getHeight() const noexcept { return height; }

            private:
                void updateSamplerState();

                std::uint32_t flags = 0;
                std::uint32_t mipmaps = 0;
                std::uint32_t sampleCount = 1;

                SamplerStateDescriptor samplerDescriptor;

                Pointer<MTLTexturePtr> texture;

                NSUInteger width = 0;
                NSUInteger height = 0;

                MTLSamplerStatePtr samplerState = nil;
                Pointer<MTLTexturePtr> msaaTexture;

                MTLPixelFormat pixelFormat;
                std::uint32_t pixelSize = 0;
                bool stencilBuffer = false;
            };
        } // namespace metal
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_METALTEXTURE_HPP
