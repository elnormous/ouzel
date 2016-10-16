// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include "graphics/opengl/RendererOGL.h"

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
            virtual void free() override;

            virtual bool present() override;

        private:
            virtual bool init(Window* newWindow,
                              uint32_t newSampleCount,
                              TextureFilter newTextureFilter,
                              PixelFormat newBackBufferFormat,
                              bool newVerticalSync) override;

            EGLDisplay display = 0;
            EGLSurface surface = 0;
            EGLContext context = 0;
        };
    } // namespace graphics
} // namespace ouzel
