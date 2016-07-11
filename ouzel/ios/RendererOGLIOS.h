// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "opengl/RendererOGL.h"

#if defined(__OBJC__)
#import <UIKit/UIKit.h>
typedef EAGLContext* EAGLContextPtr;
#else
#include <objc/objc.h>
typedef id EAGLContextPtr;
#endif

namespace ouzel
{
    class Engine;

    namespace graphics
    {
        class RendererOGLIOS: public RendererOGL
        {
            friend Engine;
        public:
            virtual ~RendererOGLIOS();
            virtual void free() override;

            virtual void clear() override;
            virtual void present() override;

        private:
            virtual bool init(const WindowPtr& window,
                              uint32_t newSampleCount,
                              TextureFiltering newTextureFiltering,
                              float newTargetFPS,
                              bool newVerticalSync) override;

            virtual void setSize(const Size2& newSize) override;

            bool createRenderBuffer(Size2& renderBufferSize);

            EAGLContextPtr context = Nil;
            GLuint colorRenderBuffer = 0;
        };
    } // namespace graphics
} // namespace ouzel
