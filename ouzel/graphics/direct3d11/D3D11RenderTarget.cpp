// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include "D3D11RenderTarget.hpp"
#include "D3D11RenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        D3D11RenderTarget::D3D11RenderTarget(D3D11RenderDevice& renderDeviceD3D11):
            D3D11RenderResource(renderDeviceD3D11)
        {
        }
    } // namespace graphics
} // namespace ouzel

#endif
