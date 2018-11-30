// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_RENDERTARGETMETAL_HPP
#define OUZEL_GRAPHICS_RENDERTARGETMETAL_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

#include "graphics/metal/RenderResourceMetal.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceMetal;

        class RenderTargetMetal final: public RenderResourceMetal
        {
        public:
            explicit RenderTargetMetal(RenderDeviceMetal& renderDeviceMetal);
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_RENDERTARGETMETAL_HPP
