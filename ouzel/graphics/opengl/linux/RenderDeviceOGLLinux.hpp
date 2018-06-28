// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "core/Setup.h"

#if OUZEL_PLATFORM_LINUX && OUZEL_COMPILE_OPENGL

#include <atomic>
#if OUZEL_OPENGL_INTERFACE_GLX
#  define GL_GLEXT_PROTOTYPES 1
#  include "GL/glx.h"
#elif OUZEL_OPENGL_INTERFACE_EGL
#  include "EGL/egl.h"
#  include "EGL/eglext.h"
#endif
#include "graphics/opengl/RenderDeviceOGL.hpp"
#include "thread/Thread.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceOGLLinux: public RenderDeviceOGL
        {
            friend Renderer;
        public:
            virtual ~RenderDeviceOGLLinux();

            virtual std::vector<Size2> getSupportedResolutions() const override;

        private:
            RenderDeviceOGLLinux();

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

#if OUZEL_OPENGL_INTERFACE_GLX
            GLXContext context = 0;
#elif OUZEL_OPENGL_INTERFACE_EGL
            EGLDisplay display = 0;
            EGLSurface surface = 0;
            EGLContext context = 0;
#endif
            std::atomic<bool> running;
            Thread renderThread;
        };
    } // namespace graphics
} // namespace ouzel

#endif
