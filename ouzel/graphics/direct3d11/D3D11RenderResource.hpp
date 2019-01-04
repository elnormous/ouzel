// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_D3D11RENDERRESOURCE_HPP
#define OUZEL_GRAPHICS_D3D11RENDERRESOURCE_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

namespace ouzel
{
    namespace graphics
    {
        class D3D11RenderDevice;

        class D3D11RenderResource
        {
        public:
            explicit D3D11RenderResource(D3D11RenderDevice& initRenderDevice):
                renderDevice(initRenderDevice)
            {
            }

            virtual ~D3D11RenderResource() {}

            D3D11RenderResource(const D3D11RenderResource&) = delete;
            D3D11RenderResource& operator=(const D3D11RenderResource&) = delete;

            D3D11RenderResource(D3D11RenderResource&&) = delete;
            D3D11RenderResource& operator=(D3D11RenderResource&&) = delete;

        protected:
            D3D11RenderDevice& renderDevice;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_D3D11RENDERRESOURCE_HPP
