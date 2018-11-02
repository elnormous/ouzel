// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "RenderDeviceEmpty.hpp"

namespace ouzel
{
    namespace graphics
    {
        RenderDeviceEmpty::RenderDeviceEmpty():
            RenderDevice(Renderer::Driver::EMPTY)
        {
        }

        void RenderDeviceEmpty::process()
        {
        }
    } // namespace graphics
} // namespace ouzel
