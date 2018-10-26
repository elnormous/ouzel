// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "core/Setup.h"

#if OUZEL_PLATFORM_IOS && OUZEL_COMPILE_METAL

#include "graphics/metal/RenderDeviceMetal.hpp"
#include "core/ios/DisplayLink.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceMetalIOS final: public RenderDeviceMetal
        {
            friend Renderer;
        public:
            RenderDeviceMetalIOS();
            virtual ~RenderDeviceMetalIOS();

            void renderCallback();

        private:
            virtual void init(Window* newWindow,
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
