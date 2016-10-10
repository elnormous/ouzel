// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "graphics/Shader.h"

namespace ouzel
{
    namespace graphics
    {
        class ShaderEmpty: public Shader
        {
        public:
            ShaderEmpty();
            virtual ~ShaderEmpty();

        protected:
            virtual bool upload() override;
        };
    } // namespace graphics
} // namespace ouzel
