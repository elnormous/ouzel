// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "graphics/opengl/RendererOGL.h"

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

            NSOpenGLContextPtr getOpenGLContext() const { return openGLContext; }
            NSOpenGLPixelFormatPtr getPixelFormat() const { return pixelFormat; }

        protected:
            virtual bool init(Window* window,
                              const Size2& newSize,
                              uint32_t newSampleCount,
                              Texture::Filter newTextureFilter,
                              PixelFormat newBackBufferFormat,
                              bool newVerticalSync,
                              bool newDepth) override;

            virtual bool upload() override;
            virtual bool lockContext() override;
            virtual bool swapBuffers() override;

            NSOpenGLContextPtr openGLContext = Nil;
            NSOpenGLPixelFormatPtr pixelFormat = Nil;
        };
    } // namespace graphics
} // namespace ouzel
