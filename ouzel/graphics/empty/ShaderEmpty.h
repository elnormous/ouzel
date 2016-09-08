// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "graphics/Shader.h"

namespace ouzel
{
    namespace graphics
    {
        class RendererEmpty;

        class ShaderEmpty: public Shader
        {
            friend RendererEmpty;
        public:
            virtual ~ShaderEmpty();

        protected:
            ShaderEmpty();
            virtual bool upload() override;
        };
    } // namespace graphics
} // namespace ouzel
