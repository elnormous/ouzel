// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "graphics/Shader.h"

namespace ouzel
{
    namespace graphics
    {
        class RendererVoid;

        class ShaderVoid: public Shader
        {
            friend RendererVoid;
        public:
            virtual ~ShaderVoid();

        protected:
            ShaderVoid();
            virtual bool upload() override;
        };
    } // namespace graphics
} // namespace ouzel
