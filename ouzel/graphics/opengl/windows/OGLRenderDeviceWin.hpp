// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_OGLRENDERDEVICEWIN_HPP
#define OUZEL_GRAPHICS_OGLRENDERDEVICEWIN_HPP

#include "core/Setup.h"

#if defined(_WIN32) && OUZEL_COMPILE_OPENGL

#include <atomic>
#include <thread>
#pragma push_macro("WIN32_LEAN_AND_MEAN")
#pragma push_macro("NOMINMAX")
#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <Windows.h>
#pragma pop_macro("WIN32_LEAN_AND_MEAN")
#pragma pop_macro("NOMINMAX")

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
                      const Size2U& newSize,
                      uint32_t newSampleCount,
                      Texture::Filter newTextureFilter,
                      uint32_t newMaxAnisotropy,
                      bool newSrgb,
                      bool newVerticalSync,
                      bool newDepth,
                      bool newStencil,
                      bool newDebugRenderer) final;

            void present() final;
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
