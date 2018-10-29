// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "graphics/RenderResource.hpp"

#pragma once

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceEmpty;

        class BlendStateResourceEmpty final: public RenderResource
        {
        public:
            explicit BlendStateResourceEmpty(RenderDeviceEmpty& renderDeviceEmpty);
        };
    } // namespace graphics
} // namespace ouzel
