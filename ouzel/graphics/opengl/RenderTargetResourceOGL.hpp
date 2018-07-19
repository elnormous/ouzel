// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "graphics/RenderTargetResource.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceOGL;

        class RenderTargetResourceOGL: public RenderTargetResource
        {
        public:
            RenderTargetResourceOGL(RenderDeviceOGL& renderDeviceOGL);
        };
    } // namespace graphics
} // namespace ouzel
