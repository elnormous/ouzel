// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef RENDERDEVICEEMPTY_HPP
#define RENDERDEVICEEMPTY_HPP

#include "graphics/RenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceEmpty final: public RenderDevice
        {
            friend Renderer;
        protected:
            RenderDeviceEmpty();

            void process() override;
        };
    } // namespace graphics
} // namespace ouzel

#endif // RENDERDEVICEEMPTY_HPP
