// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_DATATYPE_HPP
#define OUZEL_GRAPHICS_DATATYPE_HPP

#include <cstdint>

namespace ouzel
{
    namespace graphics
    {
        enum class DataType
        {
            NONE,

            BYTE,
            BYTE_NORM,
            UNSIGNED_BYTE,
            UNSIGNED_BYTE_NORM,

            BYTE_VECTOR2,
            BYTE_VECTOR2_NORM,
            UNSIGNED_BYTE_VECTOR2,
            UNSIGNED_BYTE_VECTOR2_NORM,

            BYTE_VECTOR3,
            BYTE_VECTOR3_NORM,
            UNSIGNED_BYTE_VECTOR3,
            UNSIGNED_BYTE_VECTOR3_NORM,

            BYTE_VECTOR4,
            BYTE_VECTOR4_NORM,
            UNSIGNED_BYTE_VECTOR4,
            UNSIGNED_BYTE_VECTOR4_NORM,

            SHORT,
            SHORT_NORM,
            UNSIGNED_SHORT,
            UNSIGNED_SHORT_NORM,

            SHORT_VECTOR2,
            SHORT_VECTOR2_NORM,
            UNSIGNED_SHORT_VECTOR2,
            UNSIGNED_SHORT_VECTOR2_NORM,

            SHORT_VECTOR3,
            SHORT_VECTOR3_NORM,
            UNSIGNED_SHORT_VECTOR3,
            UNSIGNED_SHORT_VECTOR3_NORM,

            SHORT_VECTOR4,
            SHORT_VECTOR4_NORM,
            UNSIGNED_SHORT_VECTOR4,
            UNSIGNED_SHORT_VECTOR4_NORM,

            INTEGER,
            UNSIGNED_INTEGER,

            INTEGER_VECTOR2,
            UNSIGNED_INTEGER_VECTOR2,

            INTEGER_VECTOR3,
            UNSIGNED_INTEGER_VECTOR3,

            INTEGER_VECTOR4,
            UNSIGNED_INTEGER_VECTOR4,

            FLOAT,
            FLOAT_VECTOR2,
            FLOAT_VECTOR3,
            FLOAT_VECTOR4,
            FLOAT_MATRIX3,
            FLOAT_MATRIX4
        };

        constexpr uint32_t getDataTypeSize(DataType dataType)
        {
            switch (dataType)
            {
                case DataType::NONE:
                    return 0;

                case DataType::BYTE:
                case DataType::BYTE_NORM:
                case DataType::UNSIGNED_BYTE:
                case DataType::UNSIGNED_BYTE_NORM:
                    return sizeof(uint8_t);
                case DataType::BYTE_VECTOR2:
                case DataType::BYTE_VECTOR2_NORM:
                case DataType::UNSIGNED_BYTE_VECTOR2:
                case DataType::UNSIGNED_BYTE_VECTOR2_NORM:
                    return 2 * sizeof(uint8_t);
                case DataType::BYTE_VECTOR3:
                case DataType::BYTE_VECTOR3_NORM:
                case DataType::UNSIGNED_BYTE_VECTOR3:
                case DataType::UNSIGNED_BYTE_VECTOR3_NORM:
                    return 3 * sizeof(uint8_t);
                case DataType::BYTE_VECTOR4:
                case DataType::BYTE_VECTOR4_NORM:
                case DataType::UNSIGNED_BYTE_VECTOR4:
                case DataType::UNSIGNED_BYTE_VECTOR4_NORM:
                    return 4 * sizeof(uint8_t);

                case DataType::SHORT:
                case DataType::SHORT_NORM:
                case DataType::UNSIGNED_SHORT:
                case DataType::UNSIGNED_SHORT_NORM:
                    return sizeof(uint16_t);
                case DataType::SHORT_VECTOR2:
                case DataType::SHORT_VECTOR2_NORM:
                case DataType::UNSIGNED_SHORT_VECTOR2:
                case DataType::UNSIGNED_SHORT_VECTOR2_NORM:
                    return 2 * sizeof(uint16_t);
                case DataType::SHORT_VECTOR3:
                case DataType::SHORT_VECTOR3_NORM:
                case DataType::UNSIGNED_SHORT_VECTOR3:
                case DataType::UNSIGNED_SHORT_VECTOR3_NORM:
                    return 3 * sizeof(uint16_t);
                case DataType::SHORT_VECTOR4:
                case DataType::SHORT_VECTOR4_NORM:
                case DataType::UNSIGNED_SHORT_VECTOR4:
                case DataType::UNSIGNED_SHORT_VECTOR4_NORM:
                    return 4 * sizeof(uint16_t);

                case DataType::INTEGER:
                case DataType::UNSIGNED_INTEGER:
                    return sizeof(uint32_t);
                case DataType::INTEGER_VECTOR2:
                case DataType::UNSIGNED_INTEGER_VECTOR2:
                    return 2 * sizeof(uint32_t);
                case DataType::INTEGER_VECTOR3:
                case DataType::UNSIGNED_INTEGER_VECTOR3:
                    return 3 * sizeof(uint32_t);
                case DataType::INTEGER_VECTOR4:
                case DataType::UNSIGNED_INTEGER_VECTOR4:
                    return 4 * sizeof(uint32_t);

                case DataType::FLOAT:
                    return sizeof(float);
                case DataType::FLOAT_VECTOR2:
                    return 2 * sizeof(float);
                case DataType::FLOAT_VECTOR3:
                    return 3 * sizeof(float);
                case DataType::FLOAT_VECTOR4:
                    return 4 * sizeof(float);
                case DataType::FLOAT_MATRIX3:
                    return 9 * sizeof(float);
                case DataType::FLOAT_MATRIX4:
                    return 16 * sizeof(float);
                default:
                    return 0;
            }
        }
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_DATATYPE_HPP
