// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_OGLRENDERDEVICEWIN_HPP
#define OUZEL_GRAPHICS_OGLRENDERDEVICEWIN_HPP

#include "core/Setup.h"

#if defined(_WIN32) && OUZEL_COMPILE_OPENGL

#include <atomic>
#include <thread>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#undef WIN32_LEAN_AND_MEAN
#undef NOMINMAX
#include "graphics/opengl/OGLRenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        class OGLRenderDeviceWin final: public OGLRenderDevice
        {
            friend Renderer;
        public:
            OGLRenderDeviceWin(const std::function<void(const Event&)>& initCallback);
            ~OGLRenderDeviceWin();

        private:
            void init(Window* newWindow,
                      const Size<2, uint32_t>& newSize,
                      uint32_t newSampleCount,
                      Texture::Filter newTextureFilter,
                      uint32_t newMaxAnisotropy,
                      bool newVerticalSync,
                      bool newDepth,
                      bool newStencil,
                      bool newDebugRenderer) override;

            void present() override;
            void main();

            HDC deviceContext = 0;
            HGLRC renderContext = 0;

            std::atomic_bool running{false};
            std::thread renderThread;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_OGLRENDERDEVICEWIN_HPP
