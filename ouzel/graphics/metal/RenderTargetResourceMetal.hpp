// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_RENDERTARGETRESOURCEMETAL_HPP
#define OUZEL_GRAPHICS_RENDERTARGETRESOURCEMETAL_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

#include "graphics/metal/RenderResourceMetal.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceMetal;

        class RenderTargetResourceMetal final: public RenderResourceMetal
        {
        public:
            explicit RenderTargetResourceMetal(RenderDeviceMetal& renderDeviceMetal);
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_RENDERTARGETRESOURCEMETAL_HPP
