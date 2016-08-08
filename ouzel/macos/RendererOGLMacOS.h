// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "opengl/RendererOGL.h"

#if defined(__OBJC__)
#import <AppKit/NSOpenGL.h>
typedef NSOpenGLContext* NSOpenGLContextPtr;
typedef NSOpenGLPixelFormat* NSOpenGLPixelFormatPtr;
#else
#include <objc/objc.h>
typedef id NSOpenGLContextPtr;
typedef id NSOpenGLPixelFormatPtr;
#endif

namespace ouzel
{
    class Engine;

    namespace graphics
    {
        class RendererOGLMacOS: public RendererOGL
        {
            friend Engine;
        public:
            virtual ~RendererOGLMacOS();
            virtual void free() override;

            virtual bool present() override;

            NSOpenGLContextPtr getOpenGLContext() const { return openGLContext; }
            NSOpenGLPixelFormatPtr getPixelFormat() const { return pixelFormat; }

        protected:
            virtual bool init(const WindowPtr& window,
                              uint32_t newSampleCount,
                              TextureFiltering newTextureFiltering,
                              float newTargetFPS,
                              bool newVerticalSync) override;

            virtual void setSize(const Size2& newSize) override;

            virtual bool update() override;
            
            NSOpenGLContextPtr openGLContext = Nil;
            NSOpenGLPixelFormatPtr pixelFormat = Nil;
        };
    } // namespace graphics
} // namespace ouzel
