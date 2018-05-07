// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/Setup.h"

#if OUZEL_PLATFORM_TVOS && OUZEL_COMPILE_METAL

#include "graphics/metal/RenderDeviceMetal.hpp"
#include "core/tvos/DisplayLink.h"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceMetalTVOS: public RenderDeviceMetal
        {
            friend Renderer;
        public:
            RenderDeviceMetalTVOS();
            virtual ~RenderDeviceMetalTVOS();

        private:
            virtual bool init(Window* newWindow,
                              const Size2& newSize,
                              uint32_t newSampleCount,
                              Texture::Filter newTextureFilter,
                              uint32_t newMaxAnisotropy,
                              bool newVerticalSync,
                              bool newDepth,
                              bool newDebugRenderer) override;

        private:
            DisplayLink displayLink;
        };
    } // namespace graphics
} // namespace ouzel

#endif
