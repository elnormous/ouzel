// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/Setup.h"

#if OUZEL_PLATFORM_IOS && OUZEL_COMPILE_METAL

#include "graphics/metal/RenderDeviceMetal.hpp"
#include "core/ios/DisplayLink.h"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceMetalIOS: public RenderDeviceMetal
        {
            friend Renderer;
        public:
            RenderDeviceMetalIOS();
            virtual ~RenderDeviceMetalIOS();

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
