// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_OPENGL

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
                              uint32_t newMaxAnisotropy,
                              bool newVerticalSync,
                              bool newDepth,
                              bool newDebugRenderer) override;
            virtual bool swapBuffers() override;

            GLXContext context = 0;
        };
    } // namespace graphics
} // namespace ouzel

#endif
