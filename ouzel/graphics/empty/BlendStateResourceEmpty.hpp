// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "graphics/empty/RenderResourceEmpty.hpp"

#pragma once

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceEmpty;

        class BlendStateResourceEmpty final: public RenderResourceEmpty
        {
        public:
            explicit BlendStateResourceEmpty(RenderDeviceEmpty& renderDeviceEmpty);
        };
    } // namespace graphics
} // namespace ouzel
