// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "RenderDeviceEmpty.hpp"

namespace ouzel
{
    namespace graphics
    {
        RenderDeviceEmpty::RenderDeviceEmpty(const std::function<void(const Event&)>& initCallback):
            RenderDevice(Driver::EMPTY, initCallback)
        {
        }

        void RenderDeviceEmpty::process()
        {
        }
    } // namespace graphics
} // namespace ouzel
