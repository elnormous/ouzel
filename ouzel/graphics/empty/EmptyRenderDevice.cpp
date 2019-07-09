// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "EmptyRenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace empty
        {
            RenderDevice::RenderDevice(const std::function<void(const Event&)>& initCallback):
                graphics::RenderDevice(Driver::Empty, initCallback)
            {
            }

            void RenderDevice::process()
            {
            }
        } // namespace empty
    } // namespace graphics
} // namespace ouzel
