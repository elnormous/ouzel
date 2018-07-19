// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "graphics/TextureResource.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceEmpty;

        class TextureResourceEmpty: public TextureResource
        {
        public:
            TextureResourceEmpty(RenderDeviceEmpty& renderDeviceEmpty);
        };
    } // namespace graphics
} // namespace ouzel
