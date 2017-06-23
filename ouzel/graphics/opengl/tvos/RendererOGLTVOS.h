// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_OPENGL

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

        private:
            virtual bool init(Window* newWindow,
                              const Size2& newSize,
                              uint32_t newSampleCount,
                              Texture::Filter newTextureFilter,
                              uint32_t newMaxAnisotropy,
                              bool newVerticalSync,
                              bool newDepth,
                              bool newDebugRenderer) override;
            virtual bool lockContext() override;
            virtual bool swapBuffers() override;
            virtual bool upload() override;

            bool createFrameBuffer();

            EAGLContextPtr context = Nil;
            CAEAGLLayerPtr eaglLayer = Nil;

            GLuint msaaFrameBufferId = 0;
            GLuint msaaColorRenderBufferId = 0;

            GLuint resolveFrameBufferId = 0;
            GLuint resolveColorRenderBufferId = 0;

            GLuint depthRenderBufferId = 0;
        };
    } // namespace graphics
} // namespace ouzel

#endif
