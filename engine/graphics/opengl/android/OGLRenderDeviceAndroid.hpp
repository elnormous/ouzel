// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_OGLRENDERDEVICEANDROID_HPP
#define OUZEL_GRAPHICS_OGLRENDERDEVICEANDROID_HPP

#include "../../../core/Setup.h"

#if defined(__ANDROID__) && OUZEL_COMPILE_OPENGL

#include <atomic>
#include <thread>
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "../OGLRenderDevice.hpp"
#include "../../../core/Window.hpp"
#include "../../../utils/Thread.hpp"

namespace ouzel::graphics::opengl::android
{
    class RenderDevice final: public opengl::RenderDevice
    {
        friend Graphics;
    public:
        RenderDevice(const Settings& settings,
                     core::Window& initWindow,
                     const std::function<void(const Event&)>& initCallback);
        ~RenderDevice() override;

        void reload();
        void destroy();

    private:
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

#endif // OUZEL_GRAPHICS_OGLRENDERDEVICEANDROID_HPP
