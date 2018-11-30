// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include "RenderTargetD3D11.hpp"
#include "RenderDeviceD3D11.hpp"

namespace ouzel
{
    namespace graphics
    {
        RenderTargetD3D11::RenderTargetD3D11(RenderDeviceD3D11& renderDeviceD3D11):
            RenderResourceD3D11(renderDeviceD3D11)
        {
        }
    } // namespace graphics
} // namespace ouzel

#endif
