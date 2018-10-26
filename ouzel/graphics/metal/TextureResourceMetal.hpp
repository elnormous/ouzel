// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

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
#  define MTLPixelFormatInvalid 0
typedef NSUInteger MTLLoadAction;
#  define MTLLoadActionDontCare 0
#endif

#include <tuple>
#include "graphics/RenderResource.hpp"
#include "graphics/Texture.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceMetal;

        class SamplerStateDescriptor
        {
        public:
            Texture::Filter filter;
            Texture::Address addressX;
            Texture::Address addressY;
            uint32_t maxAnisotropy;

            bool operator<(const SamplerStateDescriptor& other) const
            {
                return std::tie(filter, addressX, addressY, maxAnisotropy) < std::tie(other.filter, other.addressX, other.addressY, other.maxAnisotropy);
            }
        };

        class TextureResourceMetal final: public RenderResource
        {
        public:
            explicit TextureResourceMetal(RenderDeviceMetal& renderDeviceMetal);
            virtual ~TextureResourceMetal();

            void init(const std::vector<Texture::Level>& levels,
                      uint32_t newFlags = 0,
                      uint32_t newSampleCount = 1,
                      PixelFormat newPixelFormat = PixelFormat::RGBA8_UNORM);

            void setData(const std::vector<Texture::Level>& levels);
            void setFilter(Texture::Filter filter);
            void setAddressX(Texture::Address addressX);
            void setAddressY(Texture::Address addressY);
            void setMaxAnisotropy(uint32_t maxAnisotropy);
            void setClearColorBuffer(bool clear);
            void setClearDepthBuffer(bool clear);
            void setClearColor(Color color);
            void setClearDepth(float newClearDepth);

            inline uint32_t getFlags() const { return flags; }
            inline uint32_t getMipmaps() const { return mipmaps; }

            inline bool getClearColorBuffer() const { return clearColorBuffer; }
            inline bool getClearDepthBuffer() const { return clearDepthBuffer; }
            inline Color getClearColor() const { return clearColor; }
            inline float getClearDepth() const { return clearDepth; }
            inline uint32_t getSampleCount() const { return sampleCount; }
            inline PixelFormat getPixelFormat() const { return pixelFormat; }

            inline MTLTexturePtr getTexture() const { return texture; }
            inline MTLPixelFormat getColorFormat() const { return colorFormat; }
            inline MTLPixelFormat getDepthFormat() const { return depthFormat; }

            inline MTLSamplerStatePtr getSamplerState() const { return samplerState; }
            inline MTLRenderPassDescriptorPtr getRenderPassDescriptor() const { return renderPassDescriptor; }

            inline NSUInteger getWidth() const { return width; }
            inline NSUInteger getHeight() const { return height; }

            inline MTLLoadAction getColorBufferLoadAction() const { return colorBufferLoadAction; }
            inline MTLLoadAction getDepthBufferLoadAction() const { return depthBufferLoadAction; }

        private:
            void createTexture(const std::vector<Texture::Level>& levels);
            void updateSamplerState();

            uint32_t flags = 0;
            uint32_t mipmaps = 0;
            bool clearColorBuffer = true;
            bool clearDepthBuffer = false;
            Color clearColor;
            float clearDepth = 1.0F;
            uint32_t sampleCount = 1;
            PixelFormat pixelFormat = PixelFormat::RGBA8_UNORM;

            SamplerStateDescriptor samplerDescriptor;

            MTLTexturePtr texture = nil;

            NSUInteger width = 0;
            NSUInteger height = 0;

            MTLSamplerStatePtr samplerState = nil;
            MTLRenderPassDescriptorPtr renderPassDescriptor = nil;
            MTLTexturePtr msaaTexture = nil;
            MTLTexturePtr depthTexture = nil;

            MTLPixelFormat colorFormat = MTLPixelFormatInvalid;
            MTLPixelFormat depthFormat = MTLPixelFormatInvalid;

            MTLLoadAction colorBufferLoadAction = MTLLoadActionDontCare;
            MTLLoadAction depthBufferLoadAction = MTLLoadActionDontCare;
        };
    } // namespace graphics
} // namespace ouzel

#endif
