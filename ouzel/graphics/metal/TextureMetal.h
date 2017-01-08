// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#if defined(__OBJC__)
#import <Metal/Metal.h>
typedef id<MTLTexture> MTLTexturePtr;
#else
#include <objc/objc.h>
typedef id MTLTexturePtr;
#endif

#include "core/CompileConfig.h"
#include "graphics/Texture.h"

namespace ouzel
{
    namespace graphics
    {
        class RenderTargetMetal;

        class TextureMetal: public Texture
        {
            friend RenderTargetMetal;
        public:
            TextureMetal();
            virtual ~TextureMetal();
            virtual void free() override;

            MTLTexturePtr getTexture() const { return texture; }
            NSUInteger getColorFormat() const { return colorFormat; }

        protected:
            virtual bool upload() override;

            MTLTexturePtr texture = Nil;

            NSUInteger width = 0;
            NSUInteger height = 0;

            NSUInteger colorFormat = 0;
        };
    } // namespace graphics
} // namespace ouzel
