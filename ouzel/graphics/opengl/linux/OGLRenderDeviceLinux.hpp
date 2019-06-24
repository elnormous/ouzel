// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

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
        namespace opengl
        {
            class RenderDeviceLinux final: public RenderDevice
            {
                friend Renderer;
            public:
                RenderDeviceLinux(const std::function<void(const Event&)>& initCallback);
                ~RenderDeviceLinux();

                std::vector<Size2U> getSupportedResolutions() const final;

            private:
                void init(Window* newWindow,
                          const Size2U& newSize,
                          uint32_t newSampleCount,
                          SamplerFilter newTextureFilter,
                          uint32_t newMaxAnisotropy,
                          bool newSrgb,
                          bool newVerticalSync,
                          bool newDepth,
                          bool newStencil,
                          bool newDebugRenderer) final;

                void present() final;
                void renderMain();

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
        }
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_OGLRENDERDEVICELINUX_HPP
