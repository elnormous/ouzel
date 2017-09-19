// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/Setup.h"

#if OUZEL_PLATFORM_WINDOWS && OUZEL_COMPILE_OPENGL

#include <thread>
#include <atomic>
#include <windows.h>
#include "graphics/opengl/RenderDeviceOGL.hpp"

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
            virtual bool init(Window* newWindow,
                              const Size2& newSize,
                              uint32_t newSampleCount,
                              Texture::Filter newTextureFilter,
                              uint32_t newMaxAnisotropy,
                              bool newVerticalSync,
                              bool newExclusiveFullscreen,
                              bool newDepth,
                              bool newDebugRenderer) override;

            virtual bool lockContext() override;
            virtual bool swapBuffers() override;
            void main();

            HDC deviceContext = 0;
            HGLRC renderContext = 0;

            std::atomic<bool> running;
            std::thread renderThread;
        };
    } // namespace graphics
} // namespace ouzel

#endif
