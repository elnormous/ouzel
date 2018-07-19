// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "graphics/BlendStateResource.hpp"

#pragma once

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceEmpty;

        class BlendStateResourceEmpty: public BlendStateResource
        {
        public:
            BlendStateResourceEmpty(RenderDeviceEmpty& renderDeviceEmpty);
        };
    } // namespace graphics
} // namespace ouzel
