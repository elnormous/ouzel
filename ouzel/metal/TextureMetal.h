// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#ifdef __OBJC__
#import <Metal/Metal.h>
typedef id<MTLTexture> MTLTexturePtr;
#else
#include <objc/objc.h>
typedef id MTLTexturePtr;
#endif

#include "CompileConfig.h"
#include "Texture.h"

namespace ouzel
{
    namespace graphics
    {
        class RendererMetal;
        class RenderTargetMetal;

        class TextureMetal: public Texture
        {
            friend RendererMetal;
            friend RenderTargetMetal;
        public:
            virtual ~TextureMetal();

            virtual bool init(const Size2& newSize, bool newDynamic, bool newMipmaps = true, bool newRenderTarget = false) override;
            virtual bool initFromData(const void* data, const Size2& newSize, bool newDynamic, bool newMipmaps = true) override;

            virtual bool uploadMipmap(uint32_t level, const void* data) override;

            MTLTexturePtr getTexture() const { return texture; }

        protected:
            TextureMetal();

            void destroy();
            bool createTexture(NSUInteger newWidth, NSUInteger newHeight);
            bool uploadData(const void* data, const Size2& newSize) override;

            MTLTexturePtr texture = Nil;

            NSUInteger width = 0;
            NSUInteger height = 0;
        };
    } // namespace graphics
} // namespace ouzel
