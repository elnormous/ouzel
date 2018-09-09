// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

#if defined(__OBJC__)
#import <Metal/Metal.h>
typedef id<MTLSamplerState> MTLSamplerStatePtr;
typedef id<MTLTexture> MTLTexturePtr;
typedef MTLRenderPassDescriptor* MTLRenderPassDescriptorPtr;
#else
#include <objc/objc.h>
typedef id MTLSamplerStatePtr;
typedef id MTLTexturePtr;
typedef id MTLRenderPassDescriptorPtr;
typedef NSUInteger MTLPixelFormat;
#define MTLPixelFormatInvalid 0
typedef NSUInteger MTLLoadAction;
#define MTLLoadActionDontCare 0
#endif

#include "graphics/TextureResource.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceMetal;

        class TextureResourceMetal: public TextureResource
        {
        public:
            explicit TextureResourceMetal(RenderDeviceMetal& renderDeviceMetal);
            virtual ~TextureResourceMetal();

            void init(const std::vector<Texture::Level>& newLevels,
                      uint32_t newFlags = 0,
                      uint32_t newSampleCount = 1,
                      PixelFormat newPixelFormat = PixelFormat::RGBA8_UNORM);

            void setData(const std::vector<Texture::Level>& newLevels);
            void setFilter(Texture::Filter newFilter);
            void setAddressX(Texture::Address newAddressX);
            void setAddressY(Texture::Address newAddressY);
            void setMaxAnisotropy(uint32_t newMaxAnisotropy);
            void setClearColorBuffer(bool clear);
            void setClearDepthBuffer(bool clear);
            void setClearColor(Color color);
            void setClearDepth(float newClearDepth);

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
            void createTexture();
            void updateSamplerState();

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
