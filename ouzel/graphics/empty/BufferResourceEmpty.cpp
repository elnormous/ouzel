// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "BufferResourceEmpty.hpp"
#include "RenderDeviceEmpty.hpp"

namespace ouzel
{
    namespace graphics
    {
        BufferResourceEmpty::BufferResourceEmpty(RenderDeviceEmpty& renderDeviceEmpty):
            RenderResource(renderDeviceEmpty)
        {
        }
    }
}
