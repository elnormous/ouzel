// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "opengl/RendererOGL.h"

namespace ouzel
{
    class Engine;

    namespace graphics
    {
        class RendererOGLMacOS: public RendererOGL
        {
            friend Engine;
        public:
            virtual ~RendererOGLMacOS() {}

        private:
            virtual bool init(const WindowPtr& window,
                              uint32_t newSampleCount,
                              TextureFiltering newTextureFiltering,
                              float newTargetFPS,
                              bool newVerticalSync) override;
        };
    } // namespace graphics
} // namespace ouzel
