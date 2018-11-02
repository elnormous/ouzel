// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

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
