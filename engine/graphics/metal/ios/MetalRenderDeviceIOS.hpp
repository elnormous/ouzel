// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_METALRENDERDEVICEIOS_HPP
#define OUZEL_GRAPHICS_METALRENDERDEVICEIOS_HPP

#include "../../../core/Setup.h"

#if defined(__APPLE__)
#  include <TargetConditionals.h>
#endif

#if TARGET_OS_IOS && OUZEL_COMPILE_METAL

#include "../MetalRenderDevice.hpp"
#include "../../../core/ios/DisplayLink.hpp"

namespace ouzel::graphics::metal
{
    class RenderDeviceIOS final: public RenderDevice
    {
        friend Renderer;
    public:
        explicit RenderDeviceIOS(const std::function<void(const Event&)>& initCallback);
        ~RenderDeviceIOS() override;

        void renderCallback();

    private:
        void init(Window* newWindow,
                  const Size2U& newSize,
                  std::uint32_t newSampleCount,
                  bool newSrgb,
                  bool newVerticalSync,
                  bool newDepth,
                  bool newStencil,
                  bool newDebugRenderer) final;

        DisplayLink displayLink;
    };
}

#endif

#endif // OUZEL_GRAPHICS_METALRENDERDEVICEIOS_HPP
