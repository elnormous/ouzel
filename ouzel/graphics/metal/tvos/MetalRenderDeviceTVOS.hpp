// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_METALRENDERDEVICETVOS_HPP
#define OUZEL_GRAPHICS_METALRENDERDEVICETVOS_HPP

#include "core/Setup.h"

#if defined(__APPLE__)
#  include <TargetConditionals.h>
#endif

#if TARGET_OS_TV && OUZEL_COMPILE_METAL

#include "graphics/metal/MetalRenderDevice.hpp"
#include "core/tvos/DisplayLink.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace metal
        {
            class RenderDeviceTVOS final: public RenderDevice
            {
                friend Renderer;
            public:
                RenderDeviceTVOS(const std::function<void(const Event&)>& initCallback);
                ~RenderDeviceTVOS();

                void renderCallback();

            private:
                void init(Window* newWindow,
                          const Size2U& newSize,
                          uint32_t newSampleCount,
                          ouzel::graphics::Texture::Filter newTextureFilter,
                          uint32_t newMaxAnisotropy,
                          bool newVerticalSync,
                          bool newDepth,
                          bool newStencil,
                          bool newDebugRenderer) final;

                DisplayLink displayLink;
            };
        } // namespace metal
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_METALRENDERDEVICETVOS_HPP
