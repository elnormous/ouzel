// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_METALRENDERDEVICEIOS_HPP
#define OUZEL_GRAPHICS_METALRENDERDEVICEIOS_HPP

#include "core/Setup.h"

#if defined(__APPLE__)
#  include <TargetConditionals.h>
#endif

#if TARGET_OS_IOS && OUZEL_COMPILE_METAL

#include "graphics/metal/MetalRenderDevice.hpp"
#include "core/ios/DisplayLink.hpp"

namespace ouzel
{
    namespace graphics
    {
        class MetalRenderDeviceIOS final: public MetalRenderDevice
        {
            friend Renderer;
        public:
            MetalRenderDeviceIOS(const std::function<void(const Event&)>& initCallback);
            ~MetalRenderDeviceIOS();

            void renderCallback();

        private:
            void init(Window* newWindow,
                      const Size2<uint32_t>& newSize,
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

#endif // OUZEL_GRAPHICS_METALRENDERDEVICEIOS_HPP
