// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "graphics/RenderTargetResource.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceD3D11;

        class RenderTargetResourceD3D11: public RenderTargetResource
        {
        public:
            RenderTargetResourceD3D11(RenderDeviceD3D11& renderDeviceD3D11);
        };
    } // namespace graphics
} // namespace ouzel
