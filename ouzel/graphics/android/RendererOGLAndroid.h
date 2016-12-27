// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "graphics/opengl/RendererOGL.h"

namespace ouzel
{
    class Engine;

    namespace graphics
    {
        class RendererOGLAndroid: public RendererOGL
        {
            friend Engine;
        public:
            virtual ~RendererOGLAndroid();
            virtual void free() override;

        private:
            virtual bool init(Window* newWindow,
                              uint32_t newSampleCount,
                              TextureFilter newTextureFilter,
                              PixelFormat newBackBufferFormat,
                              bool newVerticalSync) override;
        };
    } // namespace graphics
} // namespace ouzel
