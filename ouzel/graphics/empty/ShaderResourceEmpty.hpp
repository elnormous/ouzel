// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "graphics/ShaderResource.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceEmpty;

        class ShaderResourceEmpty: public ShaderResource
        {
        public:
            ShaderResourceEmpty(RenderDeviceEmpty& renderDeviceEmpty);
        };
    } // namespace graphics
} // namespace ouzel
