#ifndef FLUXION_GRAPHICS_OGLRENDERRESOURCE_HPP
#define FLUXION_GRAPHICS_OGLRENDERRESOURCE_HPP

#include "../../core/Setup.h"

#if FLUXION_COMPILE_OPENGL

namespace fluxion::graphics::opengl
{
    class RenderDevice;

    class RenderResource
    {
    public:
        explicit RenderResource(RenderDevice& initRenderDevice):
            renderDevice{initRenderDevice}
        {
        }
        virtual ~RenderResource() = default;

        RenderResource(const RenderResource&) = delete;
        RenderResource& operator=(const RenderResource&) = delete;

        RenderResource(RenderResource&&) = delete;
        RenderResource& operator=(RenderResource&&) = delete;

        auto isInvalid() const noexcept { return invalid; }
        void invalidate() { invalid = true; }

        void restore()
        {
            if (invalid) reload();
            invalid = false;
        }

    protected:
        virtual void reload() = 0;

        RenderDevice& renderDevice;

    private:
        bool invalid = false;
    };
}
#endif

#endif // FLUXION_GRAPHICS_OGLRENDERRESOURCE_HPP
