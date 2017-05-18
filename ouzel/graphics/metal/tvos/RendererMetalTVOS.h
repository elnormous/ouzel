// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "graphics/metal/RendererMetal.h"

namespace ouzel
{
    namespace graphics
    {
        class RendererMetalTVOS: public RendererMetal
        {
        private:
            virtual bool init(Window* newWindow,
                              const Size2& newSize,
                              uint32_t newSampleCount,
                              Texture::Filter newTextureFilter,
                              uint32_t newMaxAnisotropy,
                              bool newVerticalSync,
                              bool newDepth,
                              bool newDebugRenderer) override;
        };
    } // namespace graphics
} // namespace ouzel
