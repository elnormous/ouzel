#ifndef FLUXION_GRAPHICS_METALRENDERRESOURCE_HPP
#define FLUXION_GRAPHICS_METALRENDERRESOURCE_HPP

#include "../../core/Setup.h"

#if FLUXION_COMPILE_METAL

namespace fluxion::graphics::metal
{
    class RenderDevice;

    class RenderResource
    {
    public:
        explicit RenderResource(RenderDevice& initRenderDevice) noexcept:
            renderDevice{initRenderDevice}
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

#endif // FLUXION_GRAPHICS_METALRENDERRESOURCE_HPP
