// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_RENDERDEVICEOGLWIN_HPP
#define OUZEL_GRAPHICS_RENDERDEVICEOGLWIN_HPP

#include "core/Setup.h"

#if OUZEL_PLATFORM_WINDOWS && OUZEL_COMPILE_OPENGL

#include <atomic>
#include <thread>
#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <Windows.h>
#include "graphics/opengl/RenderDeviceOGL.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceOGLWin final: public RenderDeviceOGL
        {
            friend Renderer;
        public:
            RenderDeviceOGLWin(const std::function<void(const Event&)>& initCallback);
            ~RenderDeviceOGLWin();

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

            HDC deviceContext = 0;
            HGLRC renderContext = 0;

            std::atomic_bool running;
            std::thread renderThread;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_RENDERDEVICEOGLWIN_HPP
