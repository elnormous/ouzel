#ifndef FLUXION_GRAPHICS_D3D11RENDERRESOURCE_HPP
#define FLUXION_GRAPHICS_D3D11RENDERRESOURCE_HPP

#include "../../core/Setup.h"

#if FLUXION_COMPILE_DIRECT3D11

namespace fluxion::graphics::d3d11
{
    class RenderDevice;

    class RenderResource
    {
    public:
        explicit RenderResource(RenderDevice& initRenderDevice):
            renderDevice(initRenderDevice)
        {
        }

        virtual ~RenderResource() = default;

        RenderResource(const RenderResource&) = delete;
        RenderResource& operator=(const RenderResource&) = delete;

        RenderResource(RenderResource&&) = delete;
        RenderResource& operator=(RenderResource&&) = delete;

    protected:
        RenderDevice& renderDevice;
    };
}
#endif

#endif // FLUXION_GRAPHICS_D3D11RENDERRESOURCE_HPP
