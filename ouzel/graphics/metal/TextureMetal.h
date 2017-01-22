// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#if defined(__OBJC__)
#import <Metal/Metal.h>
typedef id<MTLTexture> MTLTextureResourcePtr;
typedef MTLRenderPassDescriptor* MTLRenderPassDescriptorPtr;
#else
#include <objc/objc.h>
typedef id MTLTextureResourcePtr;
typedef id MTLRenderPassDescriptorPtr;
#endif

#include "core/CompileConfig.h"
#include "graphics/TextureResource.h"

namespace ouzel
{
    namespace graphics
    {
        class TextureMetal: public TextureResource
        {
        public:
            TextureMetal();
            virtual ~TextureMetal();
            virtual void free() override;

            MTLTextureResourcePtr getTexture() const { return texture; }
            NSUInteger getColorFormat() const { return colorFormat; }
            NSUInteger getDepthFormat() const { return depthFormat; }

            MTLRenderPassDescriptorPtr getRenderPassDescriptor() const { return renderPassDescriptor; }

        protected:
            virtual bool upload() override;

            MTLTextureResourcePtr texture = Nil;

            NSUInteger width = 0;
            NSUInteger height = 0;

            MTLRenderPassDescriptorPtr renderPassDescriptor = Nil;
            MTLTextureResourcePtr msaaTexture = Nil;
            MTLTextureResourcePtr depthTexture = Nil;

            NSUInteger colorFormat = 0;
            NSUInteger depthFormat = 0;
        };
    } // namespace graphics
} // namespace ouzel
