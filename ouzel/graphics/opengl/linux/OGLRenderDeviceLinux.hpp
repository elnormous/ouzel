// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_OGLRENDERDEVICELINUX_HPP
#define OUZEL_GRAPHICS_OGLRENDERDEVICELINUX_HPP

#include "core/Setup.h"

#if defined(__linux__) && !defined(__ANDROID__) && OUZEL_COMPILE_OPENGL

#include "graphics/opengl/OGL.h"

#include <atomic>
#include <thread>
#if OUZEL_OPENGL_INTERFACE_GLX
#  include "GL/glcorearb.h"
#  include <GL/glx.h>
#elif OUZEL_OPENGL_INTERFACE_EGL
#  include "EGL/egl.h"
#  include "EGL/eglext.h"
#endif
#include "graphics/opengl/OGLRenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        class OGLRenderDeviceLinux final: public OGLRenderDevice
        {
            friend Renderer;
        public:
            OGLRenderDeviceLinux(const std::function<void(const Event&)>& initCallback);
            ~OGLRenderDeviceLinux();

            std::vector<Size2<uint32_t>> getSupportedResolutions() const override;

        private:
            void init(Window* newWindow,
                      const Size2<uint32_t>& newSize,
                      uint32_t newSampleCount,
                      Texture::Filter newTextureFilter,
                      uint32_t newMaxAnisotropy,
                      bool newVerticalSync,
                      bool newDepth,
                      bool newStencil,
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
            std::atomic_bool running{false};
            std::thread renderThread;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_OGLRENDERDEVICELINUX_HPP
