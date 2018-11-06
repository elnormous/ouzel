// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

#include "RenderTargetResourceMetal.hpp"
#include "RenderDeviceMetal.hpp"

namespace ouzel
{
    namespace graphics
    {
        RenderTargetResourceMetal::RenderTargetResourceMetal(RenderDeviceMetal& renderDeviceMetal):
            RenderResourceMetal(renderDeviceMetal)
        {
        }
    } // namespace graphics
} // namespace ouzel

#endif
