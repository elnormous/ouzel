// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_RENDERRESOURCED3D11_HPP
#define OUZEL_RENDERRESOURCED3D11_HPP

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

#endif // OUZEL_RENDERRESOURCED3D11_HPP
