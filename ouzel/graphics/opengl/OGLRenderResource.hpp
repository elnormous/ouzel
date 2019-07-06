// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_OGLRENDERRESOURCE_HPP
#define OUZEL_GRAPHICS_OGLRENDERRESOURCE_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

namespace ouzel
{
    namespace graphics
    {
        namespace opengl
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

                inline auto isInvalid() const noexcept { return invalid; }
                inline void invalidate() { invalid = true; }

                inline void restore()
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
        } // namespace opengl
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_OGLRENDERRESOURCE_HPP
