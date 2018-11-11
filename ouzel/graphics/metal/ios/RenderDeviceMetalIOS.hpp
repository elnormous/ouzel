// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_RENDERDEVICEMETALIOS_HPP
#define OUZEL_GRAPHICS_RENDERDEVICEMETALIOS_HPP

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
            RenderDeviceMetalIOS(const std::function<void(const Event&)>& initCallback);
            ~RenderDeviceMetalIOS();

            void renderCallback();

        private:
            void init(Window* newWindow,
                      const Size2& newSize,
                      uint32_t newSampleCount,
                      Texture::Filter newTextureFilter,
                      uint32_t newMaxAnisotropy,
                      bool newVerticalSync,
                      bool newDepth,
                      bool newDebugRenderer) override;

            DisplayLink displayLink;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_RENDERDEVICEMETALIOS_HPP
