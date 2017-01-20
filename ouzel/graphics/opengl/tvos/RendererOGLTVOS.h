// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "graphics/opengl/RendererOGL.h"

#if defined(__OBJC__)
#import <UIKit/UIKit.h>
typedef EAGLContext* EAGLContextPtr;
typedef CAEAGLLayer* CAEAGLLayerPtr;
#else
#include <objc/objc.h>
typedef id EAGLContextPtr;
typedef id CAEAGLLayerPtr;
#endif

namespace ouzel
{
    class Engine;

    namespace graphics
    {
        class RendererOGLTVOS: public RendererOGL
        {
            friend Engine;
        public:
            virtual ~RendererOGLTVOS();
            virtual void free() override;

        private:
            virtual bool init(Window* newWindow,
                              const Size2& newSize,
                              uint32_t newSampleCount,
                              Texture::Filter newTextureFilter,
                              PixelFormat newBackBufferFormat,
                              bool newVerticalSync,
                              bool newDepth) override;
            virtual bool lockContext() override;
            virtual bool swapBuffers() override;
            
            virtual bool createFrameBuffer() override;

            EAGLContextPtr context = Nil;
            CAEAGLLayerPtr eaglLayer = Nil;

            GLuint resolveFrameBufferId = 0;
            GLuint resolveColorRenderBufferId = 0;
        };
    } // namespace graphics
} // namespace ouzel
