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

            virtual bool init(const Size2& size, bool dynamic, bool mipmaps = true) override;
            virtual bool initFromData(const void* data, const Size2& size, bool dynamic, bool mipmaps = true) override;

            virtual bool upload(const void* data, const Size2& size) override;

            MTLTexturePtr getTexture() const { return _texture; }

        protected:
            TextureMetal();

            void destroy();

            bool uploadData(const void* data, NSUInteger width, NSUInteger height);

            MTLTexturePtr _texture = Nil;
            NSUInteger _mipLevels = 0;
        };
    } // namespace graphics
} // namespace ouzel
