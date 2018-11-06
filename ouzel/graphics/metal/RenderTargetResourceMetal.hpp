// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "graphics/metal/RenderResourceMetal.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceMetal;

        class RenderTargetResourceMetal final: public RenderResourceMetal
        {
        public:
            explicit RenderTargetResourceMetal(RenderDeviceMetal& renderDeviceMetal);
        };
    } // namespace graphics
} // namespace ouzel
