// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_OPENGL

#include <windows.h>
#include "graphics/opengl/RendererOGL.h"

namespace ouzel
{
    class Engine;

    namespace graphics
    {
        class RendererOGLWin: public RendererOGL
        {
            friend Engine;
        public:
            virtual ~RendererOGLWin();

        private:
            virtual bool init(Window* newWindow,
                              const Size2& newSize,
                              uint32_t newSampleCount,
                              Texture::Filter newTextureFilter,
                              uint32_t newMaxAnisotropy,
                              bool newVerticalSync,
                              bool newDepth,
                              bool newDebugRenderer) override;

            virtual bool lockContext() override;
            virtual bool swapBuffers() override;

            HDC deviceContext = 0;
            HGLRC renderContext = 0;
        };
    } // namespace graphics
} // namespace ouzel

#endif
