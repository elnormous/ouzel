// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_OGLRENDERDEVICELINUX_HPP
#define OUZEL_GRAPHICS_OGLRENDERDEVICELINUX_HPP

#include "../../../core/Setup.h"

#if defined(__linux__) && !defined(__ANDROID__) && OUZEL_COMPILE_OPENGL

#include "../OGL.h"

#include <atomic>
#include <thread>
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "../OGLRenderDevice.hpp"
#include "../../../utils/Thread.hpp"

namespace ouzel::graphics::opengl::linux
{
    class RenderDevice final: public opengl::RenderDevice
    {
        friend Graphics;
    public:
        explicit RenderDevice(const std::function<void(const Event&)>& initCallback);
        ~RenderDevice() override;

        std::vector<Size2U> getSupportedResolutions() const final;

    private:
        void init(core::Window& newWindow,
                  const Size2U& newSize,
                  const Settings& settings) final;

        void present() final;
        void renderMain();

        EGLDisplay display = EGL_NO_DISPLAY;
        EGLSurface surface = EGL_NO_SURFACE;
        EGLContext context = EGL_NO_CONTEXT;

        std::atomic_bool running{false};
        Thread renderThread;
    };
}
#endif

#endif // OUZEL_GRAPHICS_OGLRENDERDEVICELINUX_HPP
