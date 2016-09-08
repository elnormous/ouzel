// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <mutex>
#include <atomic>

#include "core/CompileConfig.h"
#include "graphics/RenderTarget.h"
#include "math/Size2.h"

#if defined(__OBJC__)
#import <Metal/Metal.h>
typedef MTLRenderPassDescriptor* MTLRenderPassDescriptorPtr;
typedef id<MTLTexture> MTLTexturePtr;
#else
typedef id MTLRenderPassDescriptorPtr;
typedef id MTLTexturePtr;
#endif

namespace ouzel
{
    namespace graphics
    {
        class RendererMetal;

        class RenderTargetMetal: public RenderTarget
        {
            friend RendererMetal;
        public:
            virtual ~RenderTargetMetal();
            virtual void free() override;

            virtual bool init(const Size2& newSize, bool depthBuffer) override;

            virtual void setClearColor(Color color) override;

            MTLRenderPassDescriptorPtr getRenderPassDescriptor() const { return renderPassDescriptor; }

        protected:
            RenderTargetMetal();
            virtual bool upload() override;

            MTLRenderPassDescriptorPtr renderPassDescriptor = Nil;
            MTLTexturePtr msaaTexture = Nil;

            std::atomic<bool> dirty;
            std::mutex dataMutex;
        };
    } // namespace graphics
} // namespace ouzel
