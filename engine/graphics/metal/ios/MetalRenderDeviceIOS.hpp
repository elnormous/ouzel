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

namespace ouzel::graphics::metal::ios
{
    class RenderDevice final: public metal::RenderDevice
    {
        friend Graphics;
    public:
        explicit RenderDevice(const Settings& settings,
                              core::Window& initWindow,
                              const std::function<void(const Event&)>& initCallback);
        ~RenderDevice() override;

        void renderCallback();

    private:
        void init(const Settings& settings) final;

        core::ios::DisplayLink displayLink;
    };
}

#endif

#endif // OUZEL_GRAPHICS_METALRENDERDEVICEIOS_HPP
