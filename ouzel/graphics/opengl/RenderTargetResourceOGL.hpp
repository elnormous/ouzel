// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef RENDERTARGETRESOURCEOGL_HPP
#define RENDERTARGETRESOURCEOGL_HPP

#include "graphics/opengl/RenderResourceOGL.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceOGL;

        class RenderTargetResourceOGL final: public RenderResourceOGL
        {
        public:
            explicit RenderTargetResourceOGL(RenderDeviceOGL& renderDeviceOGL);

            void reload() override;
        };
    } // namespace graphics
} // namespace ouzel

#endif // RENDERTARGETRESOURCEOGL_HPP
