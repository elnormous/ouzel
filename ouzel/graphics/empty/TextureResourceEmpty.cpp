// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "TextureResourceEmpty.hpp"
#include "RenderDeviceEmpty.hpp"

namespace ouzel
{
    namespace graphics
    {
        TextureResourceEmpty::TextureResourceEmpty(RenderDeviceEmpty& renderDeviceEmpty):
            TextureResource(renderDeviceEmpty)
        {
        }
    } // namespace graphics
} // namespace ouzel
