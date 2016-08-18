// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include "opengl/RendererOGL.h"

namespace ouzel
{
    class Engine;

    namespace graphics
    {
        class RendererOGLRPI: public RendererOGL
        {
            friend Engine;
        public:
            virtual ~RendererOGLRPI();
            virtual void free() override;

            virtual bool present() override;

        private:
            virtual bool init(const WindowPtr& window,
                              uint32_t newSampleCount,
                              TextureFiltering newTextureFiltering,
                              bool newVerticalSync) override;

            EGLDisplay display = 0;
            EGLSurface surface = 0;
            EGLContext context = 0;
        };
    } // namespace graphics
} // namespace ouzel
