// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_METALRENDERDEVICETVOS_HPP
#define OUZEL_GRAPHICS_METALRENDERDEVICETVOS_HPP

#include "../../../core/Setup.h"

#if defined(__APPLE__)
#  include <TargetConditionals.h>
#endif

#if TARGET_OS_TV && OUZEL_COMPILE_METAL

#include "../MetalRenderDevice.hpp"
#include "../../../core/tvos/DisplayLink.hpp"

namespace ouzel::graphics::metal::tvos
{
    class RenderDevice final: public metal::RenderDevice
    {
        friend Graphics;
    public:
        explicit RenderDevice(const std::function<void(const Event&)>& initCallback);
        ~RenderDevice() override;

        void renderCallback();

    private:
        void init(core::Window& newWindow,
                  const Size2U& newSize,
                  const Settings& settings) final;

        core::tvos::DisplayLink displayLink;
    };
}

#endif

#endif // OUZEL_GRAPHICS_METALRENDERDEVICETVOS_HPP
