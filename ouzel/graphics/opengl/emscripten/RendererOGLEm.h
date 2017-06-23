// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_OPENGL

#include <emscripten.h>
#include <emscripten/html5.h>
#include "graphics/opengl/RendererOGL.h"

namespace ouzel
{
    class Engine;

    namespace graphics
    {
        class RendererOGLEm: public RendererOGL
        {
            friend Engine;
        public:
            virtual ~RendererOGLEm();

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

            EMSCRIPTEN_WEBGL_CONTEXT_HANDLE webGLContext = 0;
        };
    } // namespace graphics
} // namespace ouzel

#endif
