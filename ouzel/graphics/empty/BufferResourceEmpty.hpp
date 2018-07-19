// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "graphics/BufferResource.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceEmpty;

        class BufferResourceEmpty: public BufferResource
        {
        public:
            BufferResourceEmpty(RenderDeviceEmpty& renderDeviceEmpty);
        };
    } // namespace graphics
} // namespace ouzel
