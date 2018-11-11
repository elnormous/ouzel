// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_RENDERDEVICEEMPTY_HPP
#define OUZEL_GRAPHICS_RENDERDEVICEEMPTY_HPP

#include "graphics/RenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceEmpty final: public RenderDevice
        {
            friend Renderer;
        protected:
            RenderDeviceEmpty(const std::function<void(const Event&)>& initCallback);

            void process() override;
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_RENDERDEVICEEMPTY_HPP
