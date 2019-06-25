// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_OGLRENDERDEVICEANDROID_HPP
#define OUZEL_GRAPHICS_OGLRENDERDEVICEANDROID_HPP

#include "core/Setup.h"

#if defined(__ANDROID__) && OUZEL_COMPILE_OPENGL

#include <atomic>
#include <thread>
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "graphics/opengl/OGLRenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace opengl
        {
            class RenderDeviceAndroid final: public RenderDevice
            {
                friend Renderer;
            public:
                explicit RenderDeviceAndroid(const std::function<void(const Event&)>& initCallback);
                ~RenderDeviceAndroid();

                void reload();
                void destroy();

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

                EGLDisplay display = 0;
                EGLSurface surface = 0;
                EGLContext context = 0;

                std::atomic_bool running{false};
                std::thread renderThread;
            };
        } // namespace opengl
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_OGLRENDERDEVICEANDROID_HPP
