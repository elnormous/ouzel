// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_OPENGL

#include <bcm_host.h>
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "graphics/opengl/RendererOGL.h"

typedef struct {
   DISPMANX_ELEMENT_HANDLE_T element;
   int width;   /* This is necessary because dispmanx elements are not queriable. */
   int height;
} EGL_DISPMANX_WINDOW_T;

namespace ouzel
{
    class Engine;

    namespace graphics
    {
        class RendererOGLRasp: public RendererOGL
        {
            friend Engine;
        public:
            virtual ~RendererOGLRasp();

        private:
            virtual bool init(Window* newWindow,
                              const Size2& newSize,
                              uint32_t newSampleCount,
                              Texture::Filter newTextureFilter,
                              uint32_t newMaxAnisotropy,
                              bool newVerticalSync,
                              bool newDepth,
                              bool newDebugRenderer) override;
            virtual bool swapBuffers() override;

            EGLDisplay display = 0;
            EGLSurface surface = 0;
            EGLContext context = 0;

            EGL_DISPMANX_WINDOW_T nativewindow;
        };
    } // namespace graphics
} // namespace ouzel

#endif
