// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

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

            virtual ~MetalRenderResource() {}

        protected:
            MetalRenderDevice& renderDevice;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_METALRENDERRESOURCE_HPP
