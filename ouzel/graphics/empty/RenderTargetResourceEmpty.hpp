// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "graphics/empty/RenderResourceEmpty.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceEmpty;

        class RenderTargetResourceEmpty final: public RenderResourceEmpty
        {
        public:
            explicit RenderTargetResourceEmpty(RenderDeviceEmpty& renderDeviceEmpty);
        };
    } // namespace graphics
} // namespace ouzel
