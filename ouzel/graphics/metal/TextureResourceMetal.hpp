// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

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
            explicit TextureResourceMetal(RenderDeviceMetal* initRenderDeviceMetal);
            virtual ~TextureResourceMetal();

            virtual bool init(const Size2& newSize,
                              uint32_t newFlags = 0,
                              uint32_t newMipmaps = 0,
                              uint32_t newSampleCount = 1,
                              PixelFormat newPixelFormat = PixelFormat::RGBA8_UNORM) override;
            virtual bool init(const std::vector<uint8_t>& newData,
                              const Size2& newSize,
                              uint32_t newFlags = 0,
                              uint32_t newMipmaps = 0,
                              PixelFormat newPixelFormat = PixelFormat::RGBA8_UNORM) override;
            virtual bool init(const std::vector<Texture::Level>& newLevels,
                              const Size2& newSize,
                              uint32_t newFlags = 0,
                              PixelFormat newPixelFormat = PixelFormat::RGBA8_UNORM) override;

            virtual bool setSize(const Size2& newSize) override;
            virtual bool setData(const std::vector<uint8_t>& newData, const Size2& newSize) override;
            virtual bool setFilter(Texture::Filter newFilter) override;
            virtual bool setAddressX(Texture::Address newAddressX) override;
            virtual bool setAddressY(Texture::Address newAddressY) override;
            virtual bool setMaxAnisotropy(uint32_t newMaxAnisotropy) override;
            virtual bool setClearColorBuffer(bool clear) override;
            virtual bool setClearDepthBuffer(bool clear) override;
            virtual bool setClearColor(Color color) override;
            virtual bool setClearDepth(float clear) override;

            inline MTLTexturePtr getTexture() const { return texture; }
            inline MTLPixelFormat getColorFormat() const { return colorFormat; }
            inline MTLPixelFormat getDepthFormat() const { return depthFormat; }

            inline MTLSamplerStatePtr getSamplerState() const { return samplerState; }
            inline MTLRenderPassDescriptorPtr getRenderPassDescriptor() const { return renderPassDescriptor; }

            inline NSUInteger getWidth() const { return width; }
            inline NSUInteger getHeight() const { return height; }

            inline MTLLoadAction getColorBufferLoadAction() const { return colorBufferLoadAction; }
            inline MTLLoadAction getDepthBufferLoadAction() const { return depthBufferLoadAction; }

        protected:
            bool createTexture();
            bool updateSamplerState();

            RenderDeviceMetal* renderDeviceMetal;

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
