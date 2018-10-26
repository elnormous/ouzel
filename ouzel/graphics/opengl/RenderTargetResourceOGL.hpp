// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "graphics/opengl/RenderResourceOGL.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceOGL;

        class RenderTargetResourceOGL final: public RenderResourceOGL
        {
        public:
            RenderTargetResourceOGL(RenderDeviceOGL& renderDeviceOGL);

            virtual void reload() override;
        };
    } // namespace graphics
} // namespace ouzel
