// Copyright (C) 2016 Elviss Strazdins
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

            virtual void setSize(const Size2& newSize) override;

            virtual bool present() override;

        private:
            virtual bool init(Window* newWindow,
                              uint32_t newSampleCount,
                              TextureFilter newTextureFilter,
                              PixelFormat newBackBufferFormat,
                              bool newVerticalSync) override;

            bool createRenderBuffer(Size2& renderBufferSize);

            EAGLContextPtr context = Nil;
            GLuint colorRenderBuffer = 0;

            CAEAGLLayerPtr eaglLayer = Nil;
        };
    } // namespace graphics
} // namespace ouzel
