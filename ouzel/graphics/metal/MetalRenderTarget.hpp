// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_METALRENDERTARGET_HPP
#define OUZEL_GRAPHICS_METALRENDERTARGET_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

#include "graphics/metal/MetalRenderResource.hpp"

namespace ouzel
{
    namespace graphics
    {
        class MetalRenderDevice;

        class MetalRenderTarget final: public MetalRenderResource
        {
        public:
            explicit MetalRenderTarget(MetalRenderDevice& renderDeviceMetal);
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_METALRENDERTARGET_HPP
