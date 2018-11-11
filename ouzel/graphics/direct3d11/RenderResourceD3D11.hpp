// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_RENDERRESOURCED3D11_HPP
#define OUZEL_GRAPHICS_RENDERRESOURCED3D11_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceD3D11;

        class RenderResourceD3D11
        {
        public:
            explicit RenderResourceD3D11(RenderDeviceD3D11& initRenderDevice):
                renderDevice(initRenderDevice)
            {
            }

            virtual ~RenderResourceD3D11() {}

        protected:
            RenderDeviceD3D11& renderDevice;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_RENDERRESOURCED3D11_HPP
