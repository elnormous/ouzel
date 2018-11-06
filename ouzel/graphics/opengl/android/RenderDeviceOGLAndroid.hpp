// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "core/Setup.h"

#if OUZEL_PLATFORM_ANDROID && OUZEL_COMPILE_OPENGL

#include <atomic>
#include <thread>
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "graphics/opengl/RenderDeviceOGL.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceOGLAndroid final: public RenderDeviceOGL
        {
            friend Renderer;
        public:
            ~RenderDeviceOGLAndroid();

            void reload();
            void destroy();

        private:
            void init(Window* newWindow,
                      const Size2& newSize,
                      uint32_t newSampleCount,
                      Texture::Filter newTextureFilter,
                      uint32_t newMaxAnisotropy,
                      bool newVerticalSync,
                      bool newDepth,
                      bool newDebugRenderer) override;

            void present() override;
            void main();

            EGLDisplay display = 0;
            EGLSurface surface = 0;
            EGLContext context = 0;

            std::atomic_bool running;
            std::thread renderThread;
        };
    } // namespace graphics
} // namespace ouzel

#endif
