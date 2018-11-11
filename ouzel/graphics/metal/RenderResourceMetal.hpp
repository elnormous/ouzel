// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_RENDERRESOURCEMETAL_HPP
#define OUZEL_GRAPHICS_RENDERRESOURCEMETAL_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceMetal;

        class RenderResourceMetal
        {
        public:
            explicit RenderResourceMetal(RenderDeviceMetal& initRenderDevice):
                renderDevice(initRenderDevice)
            {
            }

            virtual ~RenderResourceMetal() {}

        protected:
            RenderDeviceMetal& renderDevice;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_RENDERRESOURCEMETAL_HPP
