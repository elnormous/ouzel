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
            Byte,
            ByteNorm,
            UnsignedByte,
            UnsignedByteNorm,

            ByteVector2,
            ByteVector2Norm,
            UnsignedByteVector2,
            UnsignedByteVector2Norm,

            ByteVector3,
            ByteVector3Norm,
            UnsignedByteVector3,
            UnsignedByteVector3Norm,

            ByteVector4,
            ByteVector4Norm,
            UnsignedByteVector4,
            UnsignedByteVector4Norm,

            Short,
            ShortNorm,
            UnsignedShort,
            UnsignedShortNorm,

            ShortVector2,
            ShortVector2Norm,
            UnsignedShortVector2,
            UnsignedShortVector2Norm,

            ShortVector3,
            ShortVector3Norm,
            UnsignedShortVector3,
            UnsignedShortVector3Norm,

            ShortVector4,
            ShortVector4Norm,
            UnsignedShortVector4,
            UnsignedShortVector4Norm,

            Integer,
            UnsignedInteger,

            IntegerVector2,
            UnsignedIntegerVector2,

            IntegerVector3,
            UnsignedIntegerVector3,

            IntegerVector4,
            UnsignedIntegerVector4,

            Float,
            FloatVector2,
            FloatVector3,
            FloatVector4,
            FloatMatrix3,
            FloatMatrix4
        };

        inline uint32_t getDataTypeSize(DataType dataType)
        {
            switch (dataType)
            {
                case DataType::Byte:
                case DataType::ByteNorm:
                case DataType::UnsignedByte:
                case DataType::UnsignedByteNorm:
                    return sizeof(uint8_t);
                case DataType::ByteVector2:
                case DataType::ByteVector2Norm:
                case DataType::UnsignedByteVector2:
                case DataType::UnsignedByteVector2Norm:
                    return 2 * sizeof(uint8_t);
                case DataType::ByteVector3:
                case DataType::ByteVector3Norm:
                case DataType::UnsignedByteVector3:
                case DataType::UnsignedByteVector3Norm:
                    return 3 * sizeof(uint8_t);
                case DataType::ByteVector4:
                case DataType::ByteVector4Norm:
                case DataType::UnsignedByteVector4:
                case DataType::UnsignedByteVector4Norm:
                    return 4 * sizeof(uint8_t);

                case DataType::Short:
                case DataType::ShortNorm:
                case DataType::UnsignedShort:
                case DataType::UnsignedShortNorm:
                    return sizeof(uint16_t);
                case DataType::ShortVector2:
                case DataType::ShortVector2Norm:
                case DataType::UnsignedShortVector2:
                case DataType::UnsignedShortVector2Norm:
                    return 2 * sizeof(uint16_t);
                case DataType::ShortVector3:
                case DataType::ShortVector3Norm:
                case DataType::UnsignedShortVector3:
                case DataType::UnsignedShortVector3Norm:
                    return 3 * sizeof(uint16_t);
                case DataType::ShortVector4:
                case DataType::ShortVector4Norm:
                case DataType::UnsignedShortVector4:
                case DataType::UnsignedShortVector4Norm:
                    return 4 * sizeof(uint16_t);

                case DataType::Integer:
                case DataType::UnsignedInteger:
                    return sizeof(uint32_t);
                case DataType::IntegerVector2:
                case DataType::UnsignedIntegerVector2:
                    return 2 * sizeof(uint32_t);
                case DataType::IntegerVector3:
                case DataType::UnsignedIntegerVector3:
                    return 3 * sizeof(uint32_t);
                case DataType::IntegerVector4:
                case DataType::UnsignedIntegerVector4:
                    return 4 * sizeof(uint32_t);

                case DataType::Float:
                    return sizeof(float);
                case DataType::FloatVector2:
                    return 2 * sizeof(float);
                case DataType::FloatVector3:
                    return 3 * sizeof(float);
                case DataType::FloatVector4:
                    return 4 * sizeof(float);
                case DataType::FloatMatrix3:
                    return 9 * sizeof(float);
                case DataType::FloatMatrix4:
                    return 16 * sizeof(float);
                default:
                    return 0;
            }
        }
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_DATATYPE_HPP
