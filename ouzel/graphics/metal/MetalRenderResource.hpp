// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_METALRENDERRESOURCE_HPP
#define OUZEL_GRAPHICS_METALRENDERRESOURCE_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

namespace ouzel
{
    namespace graphics
    {
        class MetalRenderDevice;

        class MetalRenderResource
        {
        public:
            explicit MetalRenderResource(MetalRenderDevice& initRenderDevice):
                renderDevice(initRenderDevice)
            {
            }

            virtual ~MetalRenderResource() = default;

            MetalRenderResource(const MetalRenderResource&) = delete;
            MetalRenderResource& operator=(const MetalRenderResource&) = delete;

            MetalRenderResource(MetalRenderResource&&) = delete;
            MetalRenderResource& operator=(MetalRenderResource&&) = delete;

        protected:
            MetalRenderDevice& renderDevice;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_METALRENDERRESOURCE_HPP
