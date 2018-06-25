// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/Setup.h"

#if OUZEL_PLATFORM_ANDROID && OUZEL_COMPILE_OPENGL

#include <atomic>
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "graphics/opengl/RenderDeviceOGL.hpp"
#include "thread/Thread.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceOGLAndroid: public RenderDeviceOGL
        {
            friend Renderer;
        public:
            virtual ~RenderDeviceOGLAndroid();

            bool reload();
            bool destroy();

        private:
            virtual void init(Window* newWindow,
                              const Size2& newSize,
                              uint32_t newSampleCount,
                              Texture::Filter newTextureFilter,
                              uint32_t newMaxAnisotropy,
                              bool newVerticalSync,
                              bool newDepth,
                              bool newDebugRenderer) override;

            virtual void lockContext() override;
            virtual void swapBuffers() override;
            void main();

            EGLDisplay display = 0;
            EGLSurface surface = 0;
            EGLContext context = 0;

            std::atomic<bool> running;
            Thread renderThread;
        };
    } // namespace graphics
} // namespace ouzel

#endif
