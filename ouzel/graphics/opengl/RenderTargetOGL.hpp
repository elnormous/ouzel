// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_RENDERTARGETOGL_HPP
#define OUZEL_GRAPHICS_RENDERTARGETOGL_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "graphics/opengl/RenderResourceOGL.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceOGL;

        class RenderTargetOGL final: public RenderResourceOGL
        {
        public:
            explicit RenderTargetOGL(RenderDeviceOGL& renderDeviceOGL);

            void reload() override;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_RENDERTARGETOGL_HPP
