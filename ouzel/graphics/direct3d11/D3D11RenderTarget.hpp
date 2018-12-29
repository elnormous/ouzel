// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_D3D11RENDERTARGET_HPP
#define OUZEL_GRAPHICS_D3D11RENDERTARGET_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include "graphics/direct3d11/D3D11RenderResource.hpp"

namespace ouzel
{
    namespace graphics
    {
        class D3D11RenderDevice;

        class D3D11RenderTarget final: public D3D11RenderResource
        {
        public:
            explicit D3D11RenderTarget(D3D11RenderDevice& renderDeviceD3D11);
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_D3D11RENDERTARGET_HPP
