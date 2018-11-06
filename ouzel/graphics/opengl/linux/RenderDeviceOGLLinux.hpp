// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef RENDERDEVICEOGLLINUX_HPP
#define RENDERDEVICEOGLLINUX_HPP

#include "core/Setup.h"

#if OUZEL_PLATFORM_LINUX && OUZEL_COMPILE_OPENGL

#include <atomic>
#include <thread>
#if OUZEL_OPENGL_INTERFACE_GLX
#  include "GL/glcorearb.h"
#  include <GL/glx.h>
#elif OUZEL_OPENGL_INTERFACE_EGL
#  include "EGL/egl.h"
#  include "EGL/eglext.h"
#endif
#include "graphics/opengl/RenderDeviceOGL.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceOGLLinux final: public RenderDeviceOGL
        {
            friend Renderer;
        public:
            ~RenderDeviceOGLLinux();

            std::vector<Size2> getSupportedResolutions() const override;

        private:
            RenderDeviceOGLLinux();

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

#if OUZEL_OPENGL_INTERFACE_GLX
            GLXContext context = 0;
#elif OUZEL_OPENGL_INTERFACE_EGL
            EGLDisplay display = 0;
            EGLSurface surface = 0;
            EGLContext context = 0;
#endif
            std::atomic_bool running;
            std::thread renderThread;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // RENDERDEVICEOGLLINUX_HPP
