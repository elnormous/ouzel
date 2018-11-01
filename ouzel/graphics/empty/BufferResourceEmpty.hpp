// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "graphics/empty/RenderResourceEmpty.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceEmpty;

        class BufferResourceEmpty final: public RenderResourceEmpty
        {
        public:
            explicit BufferResourceEmpty(RenderDeviceEmpty& renderDeviceEmpty);
        };
    } // namespace graphics
} // namespace ouzel
