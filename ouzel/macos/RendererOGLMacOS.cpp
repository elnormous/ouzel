// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RendererOGLMacOS.h"

namespace ouzel
{
    namespace graphics
    {
        bool RendererOGLMacOS::init(const WindowPtr& window,
                                  uint32_t newSampleCount,
                                  TextureFiltering newTextureFiltering,
                                  float newTargetFPS,
                                  bool newVerticalSync)
        {
            free();

            return RendererOGL::init(window, newSampleCount, newTextureFiltering, newTargetFPS, newVerticalSync);
        }
    } // namespace graphics
} // namespace ouzel
