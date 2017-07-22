// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_METAL

#if defined(__OBJC__)
#import <Metal/Metal.h>
typedef id<MTLSamplerState> MTLSamplerStatePtr;
typedef id<MTLTexture> MTLTextureResourcePtr;
typedef MTLRenderPassDescriptor* MTLRenderPassDescriptorPtr;
#else
#include <objc/objc.h>
typedef id MTLSamplerStatePtr;
typedef id MTLTextureResourcePtr;
typedef id MTLRenderPassDescriptorPtr;
typedef NSUInteger MTLPixelFormat;
#define MTLPixelFormatInvalid 0
typedef NSUInteger MTLLoadAction;
#define MTLLoadActionDontCare 0
#endif

#include "core/CompileConfig.h"
#include "graphics/TextureResource.h"

namespace ouzel
{
    namespace graphics
    {
        class TextureResourceMetal: public TextureResource
        {
        public:
            TextureResourceMetal();
            virtual ~TextureResourceMetal();

            virtual bool init(const Size2& newSize,
                              bool newDynamic,
                              bool newMipmaps = true,
                              bool newRenderTarget = false,
                              uint32_t newSampleCount = 1,
                              bool newDepth = false,
                              PixelFormat newPixelFormat = PixelFormat::RGBA8_UNORM) override;
            virtual bool init(const std::vector<uint8_t>& newData,
                              const Size2& newSize,
                              bool newDynamic,
                              bool newMipmaps = true,
                              PixelFormat newPixelFormat = PixelFormat::RGBA8_UNORM) override;

            virtual bool setSize(const Size2& newSize) override;
            virtual bool setData(const std::vector<uint8_t>& newData, const Size2& newSize) override;
            virtual bool setFilter(Texture::Filter newFilter) override;
            virtual bool setAddressX(Texture::Address newAddressX) override;
            virtual bool setAddressY(Texture::Address newAddressY) override;
            virtual bool setMaxAnisotropy(uint32_t newMaxAnisotropy) override;
            virtual void setClearColorBuffer(bool clear) override;
            virtual void setClearDepthBuffer(bool clear) override;
            virtual void setClearColor(Color color) override;

            MTLTextureResourcePtr getTexture() const { return texture; }
            MTLPixelFormat getColorFormat() const { return colorFormat; }
            MTLPixelFormat getDepthFormat() const { return depthFormat; }

            MTLSamplerStatePtr getSamplerState() const { return samplerState; }
            MTLRenderPassDescriptorPtr getRenderPassDescriptor() const { return renderPassDescriptor; }

            NSUInteger getWidth() const { return width; }
            NSUInteger getHeight() const { return height; }

            MTLLoadAction getColorBufferLoadAction() const { return colorBufferLoadAction; }
            MTLLoadAction getDepthBufferLoadAction() const { return depthBufferLoadAction; }

        protected:
            virtual bool upload() override;

            MTLTextureResourcePtr texture = Nil;

            NSUInteger width = 0;
            NSUInteger height = 0;

            MTLSamplerStatePtr samplerState = Nil;
            MTLRenderPassDescriptorPtr renderPassDescriptor = Nil;
            MTLTextureResourcePtr msaaTexture = Nil;
            MTLTextureResourcePtr depthTexture = Nil;

            MTLPixelFormat colorFormat = MTLPixelFormatInvalid;
            MTLPixelFormat depthFormat = MTLPixelFormatInvalid;

            MTLLoadAction colorBufferLoadAction = MTLLoadActionDontCare;
            MTLLoadAction depthBufferLoadAction = MTLLoadActionDontCare;
        };
    } // namespace graphics
} // namespace ouzel

#endif
