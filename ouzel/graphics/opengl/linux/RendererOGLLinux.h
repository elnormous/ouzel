// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#define GL_GLEXT_PROTOTYPES 1
#include <GL/glx.h>
#include "graphics/opengl/RendererOGL.h"

namespace ouzel
{
    class Engine;

    namespace graphics
    {
        class RendererOGLLinux: public RendererOGL
        {
            friend Engine;
        public:
            virtual ~RendererOGLLinux();

        private:
            virtual bool init(Window* newWindow,
                              const Size2& newSize,
                              uint32_t newSampleCount,
                              Texture::Filter newTextureFilter,
                              PixelFormat newBackBufferFormat,
                              bool newVerticalSync,
                              bool newDepth) override;
            virtual bool swapBuffers() override;

            GLXContext context = 0;
        };
    } // namespace graphics
} // namespace ouzel
