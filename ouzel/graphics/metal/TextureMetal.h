// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#if defined(__OBJC__)
#import <Metal/Metal.h>
typedef id<MTLTexture> MTLTexturePtr;
typedef MTLRenderPassDescriptor* MTLRenderPassDescriptorPtr;
#else
#include <objc/objc.h>
typedef id MTLTexturePtr;
typedef id MTLRenderPassDescriptorPtr;
#endif

#include "core/CompileConfig.h"
#include "graphics/Texture.h"

namespace ouzel
{
    namespace graphics
    {
        class TextureMetal: public Texture
        {
        public:
            TextureMetal();
            virtual ~TextureMetal();
            virtual void free() override;

            MTLTexturePtr getTexture() const { return texture; }
            NSUInteger getColorFormat() const { return colorFormat; }
            NSUInteger getDepthFormat() const { return depthFormat; }

            MTLRenderPassDescriptorPtr getRenderPassDescriptor() const { return renderPassDescriptor; }

        protected:
            virtual bool upload() override;

            MTLTexturePtr texture = Nil;

            NSUInteger width = 0;
            NSUInteger height = 0;

            MTLRenderPassDescriptorPtr renderPassDescriptor = Nil;
            MTLTexturePtr msaaTexture = Nil;

            NSUInteger colorFormat = 0;
            NSUInteger depthFormat = 0;
        };
    } // namespace graphics
} // namespace ouzel
