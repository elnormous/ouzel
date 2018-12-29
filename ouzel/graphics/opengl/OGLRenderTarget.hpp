// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_OGLRENDERTARGET_HPP
#define OUZEL_GRAPHICS_OGLRENDERTARGET_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "graphics/opengl/OGLRenderResource.hpp"

namespace ouzel
{
    namespace graphics
    {
        class OGLRenderDevice;

        class OGLRenderTarget final: public OGLRenderResource
        {
        public:
            explicit OGLRenderTarget(OGLRenderDevice& renderDeviceOGL);

            void reload() override;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_OGLRENDERTARGET_HPP
