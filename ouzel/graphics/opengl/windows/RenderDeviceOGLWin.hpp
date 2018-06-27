// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "core/Setup.h"

#if OUZEL_PLATFORM_WINDOWS && OUZEL_COMPILE_OPENGL

#include <atomic>
#include <Windows.h>
#include "graphics/opengl/RenderDeviceOGL.hpp"
#include "thread/Thread.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceOGLWin: public RenderDeviceOGL
        {
            friend Renderer;
        public:
            virtual ~RenderDeviceOGLWin();

        private:
            RenderDeviceOGLWin();
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

            HDC deviceContext = 0;
            HGLRC renderContext = 0;

            std::atomic<bool> running;
            Thread renderThread;
        };
    } // namespace graphics
} // namespace ouzel

#endif
