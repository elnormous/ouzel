// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <mutex>

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
        class RendererMetal;
        class RenderTargetMetal;

        class TextureMetal: public Texture
        {
            friend RendererMetal;
            friend RenderTargetMetal;
        public:
            virtual ~TextureMetal();
            virtual void free() override;

            virtual bool init(const Size2& newSize, bool newDynamic, bool newMipmaps = true, bool newRenderTarget = false) override;
            virtual bool initFromBuffer(const std::vector<uint8_t>& newData, const Size2& newSize, bool newDynamic, bool newMipmaps = true) override;

            virtual bool upload(const std::vector<uint8_t>& newData, const Size2& newSize) override;

            MTLTexturePtr getTexture() const { return texture; }

        protected:
            TextureMetal();
            virtual bool update() override;

            virtual bool uploadData(const std::vector<uint8_t>& newData, const Size2& newSize) override;
            virtual bool uploadMipmap(uint32_t level, const Size2& mipMapSize, const std::vector<uint8_t>& newData) override;

            MTLTexturePtr texture = Nil;

            NSUInteger width = 0;
            NSUInteger height = 0;

            struct Data
            {
                NSUInteger width = 0;
                NSUInteger height = 0;
                std::vector<uint8_t> data;
            };

            std::vector<Data> data;
            std::atomic<bool> dirty;
            std::mutex dataMutex;
        };
    } // namespace graphics
} // namespace ouzel
