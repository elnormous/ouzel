// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

#include "RenderTargetMetal.hpp"
#include "RenderDeviceMetal.hpp"

namespace ouzel
{
    namespace graphics
    {
        RenderTargetMetal::RenderTargetMetal(RenderDeviceMetal& renderDeviceMetal):
            RenderResourceMetal(renderDeviceMetal)
        {
        }
    } // namespace graphics
} // namespace ouzel

#endif
