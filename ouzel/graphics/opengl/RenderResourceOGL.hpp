// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_RENDERRESOURCEOGL_HPP
#define OUZEL_GRAPHICS_RENDERRESOURCEOGL_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceOGL;

        class RenderResourceOGL
        {
        public:
            explicit RenderResourceOGL(RenderDeviceOGL& initRenderDevice):
                renderDevice(initRenderDevice)
            {
            }
            virtual ~RenderResourceOGL() {}

            virtual void reload() = 0;

        protected:
            RenderDeviceOGL& renderDevice;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_RENDERRESOURCEOGL_HPP
