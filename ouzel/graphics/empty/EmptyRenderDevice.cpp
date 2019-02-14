// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "EmptyRenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        EmptyRenderDevice::EmptyRenderDevice(const std::function<void(const Event&)>& initCallback):
            RenderDevice(Driver::EMPTY, initCallback)
        {
        }

        void EmptyRenderDevice::process()
        {
        }
    } // namespace graphics
} // namespace ouzel
