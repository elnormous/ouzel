// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

namespace ouzel
{
    namespace graphics
    {
        enum class DataType
        {
            BYTE,
            BYTE_VECTOR2,
            BYTE_VECTOR3,
            BYTE_VECTOR4,
            UNSIGNED_BYTE,
            UNSIGNED_BYTE_VECTOR2,
            UNSIGNED_BYTE_VECTOR3,
            UNSIGNED_BYTE_VECTOR4,

            SHORT,
            SHORT_VECTOR2,
            SHORT_VECTOR3,
            SHORT_VECTOR4,
            UNSIGNED_SHORT,
            UNSIGNED_SHORT_VECTOR2,
            UNSIGNED_SHORT_VECTOR3,
            UNSIGNED_SHORT_VECTOR4,

            INTEGER,
            INTEGER_VECTOR2,
            INTEGER_VECTOR3,
            INTEGER_VECTOR4,
            UNSIGNED_INTEGER,
            UNSIGNED_INTEGER_VECTOR2,
            UNSIGNED_INTEGER_VECTOR3,
            UNSIGNED_INTEGER_VECTOR4,

            FLOAT,
            FLOAT_VECTOR2,
            FLOAT_VECTOR3,
            FLOAT_VECTOR4,
            FLOAT_MATRIX3,
            FLOAT_MATRIX4
        };
    } // namespace graphics
} // namespace ouzel
