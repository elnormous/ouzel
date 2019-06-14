// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_METALTEXTURE_HPP
#define OUZEL_GRAPHICS_METALTEXTURE_HPP

#include "core/Setup.h"

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
#include "graphics/metal/MetalRenderResource.hpp"
#include "graphics/Texture.hpp"

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
                uint32_t maxAnisotropy;

                bool operator<(const SamplerStateDescriptor& other) const
                {
                    return std::tie(filter, addressX, addressY, addressZ, maxAnisotropy) <
                        std::tie(other.filter, other.addressX, other.addressY, other.addressZ, other.maxAnisotropy);
                }
            };

            class Texture final: public RenderResource
            {
            public:
                Texture(RenderDevice& renderDeviceMetal,
                        const std::vector<graphics::Texture::Level>& levels,
                        TextureType type,
                        uint32_t initFlags = 0,
                        uint32_t initSampleCount = 1,
                        PixelFormat initPixelFormat = PixelFormat::RGBA8_UNORM);
                ~Texture();

                void setData(const std::vector<graphics::Texture::Level>& levels);
                void setFilter(SamplerFilter filter);
                void setAddressX(SamplerAddressMode addressX);
                void setAddressY(SamplerAddressMode addressY);
                void setAddressZ(SamplerAddressMode addressZ);
                void setMaxAnisotropy(uint32_t maxAnisotropy);

                inline uint32_t getFlags() const { return flags; }
                inline uint32_t getMipmaps() const { return mipmaps; }
                inline uint32_t getSampleCount() const { return sampleCount; }

                inline MTLTexturePtr getTexture() const { return texture; }
                inline MTLPixelFormat getPixelFormat() const { return pixelFormat; }
                inline bool getStencilBuffer() const { return stencilBuffer; }

                inline MTLSamplerStatePtr getSamplerState() const { return samplerState; }

                inline NSUInteger getWidth() const { return width; }
                inline NSUInteger getHeight() const { return height; }

            private:
                void updateSamplerState();

                uint32_t flags = 0;
                uint32_t mipmaps = 0;
                uint32_t sampleCount = 1;

                SamplerStateDescriptor samplerDescriptor;

                MTLTexturePtr texture = nil;

                NSUInteger width = 0;
                NSUInteger height = 0;

                MTLSamplerStatePtr samplerState = nil;
                MTLTexturePtr msaaTexture = nil;

                MTLPixelFormat pixelFormat;
                bool stencilBuffer = false;
            };
        } // namespace metal
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_METALTEXTURE_HPP
