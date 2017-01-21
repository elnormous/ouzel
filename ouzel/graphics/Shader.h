// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include "utils/Noncopyable.h"

namespace ouzel
{
    namespace graphics
    {
        class ShaderResource;

        class Shader: public Noncopyable
        {
        public:
            enum class DataType
            {
                FLOAT,
                FLOAT_VECTOR2,
                FLOAT_VECTOR3,
                FLOAT_VECTOR4,
                FLOAT_MATRIX3,
                FLOAT_MATRIX4
            };
            
            Shader();

        private:
            std::unique_ptr<ShaderResource> resource;
        };
    } // namespace graphics
} // namespace ouzel
