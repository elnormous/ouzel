// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "graphics/RenderResource.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceMetal;

        class RenderTargetResourceMetal: public RenderResource
        {
        public:
            RenderTargetResourceMetal(RenderDeviceMetal& renderDeviceMetal);
        };
    } // namespace graphics
} // namespace ouzel
