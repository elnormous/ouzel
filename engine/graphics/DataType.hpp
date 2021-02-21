// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_DATATYPE_HPP
#define OUZEL_GRAPHICS_DATATYPE_HPP

#include <cstdint>

namespace ouzel::graphics
{
    enum class DataType
    {
        byte,
        byteNorm,
        unsignedByte,
        unsignedByteNorm,

        byteVector2,
        byteVector2Norm,
        unsignedByteVector2,
        unsignedByteVector2Norm,

        byteVector3,
        byteVector3Norm,
        unsignedByteVector3,
        unsignedByteVector3Norm,

        byteVector4,
        byteVector4Norm,
        unsignedByteVector4,
        unsignedByteVector4Norm,

        integer16,
        integer16Norm,
        unsignedInteger16,
        unsignedInteger16Norm,

        integer16Vector2,
        integer16Vector2Norm,
        unsignedInteger16Vector2,
        unsignedInteger16Vector2Norm,

        integer16Vector3,
        integer16Vector3Norm,
        unsignedInteger16Vector3,
        unsignedInteger16Vector3Norm,

        integer16Vector4,
        integer16Vector4Norm,
        unsignedInteger16Vector4,
        unsignedInteger16Vector4Norm,

        integer32,
        unsignedInteger32,

        integer32Vector2,
        unsignedInteger32Vector2,

        integer32Vector3,
        unsignedInteger32Vector3,

        integer32Vector4,
        unsignedInteger32Vector4,

        float32,
        float32Vector2,
        float32Vector3,
        float32Vector4,
        float32Matrix3,
        float32Matrix4
    };

    inline std::uint32_t getDataTypeSize(DataType dataType)
    {
        switch (dataType)
        {
            case DataType::byte:
            case DataType::byteNorm:
            case DataType::unsignedByte:
            case DataType::unsignedByteNorm:
                return sizeof(std::uint8_t);
            case DataType::byteVector2:
            case DataType::byteVector2Norm:
            case DataType::unsignedByteVector2:
            case DataType::unsignedByteVector2Norm:
                return 2 * sizeof(std::uint8_t);
            case DataType::byteVector3:
            case DataType::byteVector3Norm:
            case DataType::unsignedByteVector3:
            case DataType::unsignedByteVector3Norm:
                return 3 * sizeof(std::uint8_t);
            case DataType::byteVector4:
            case DataType::byteVector4Norm:
            case DataType::unsignedByteVector4:
            case DataType::unsignedByteVector4Norm:
                return 4 * sizeof(std::uint8_t);

            case DataType::integer16:
            case DataType::integer16Norm:
            case DataType::unsignedInteger16:
            case DataType::unsignedInteger16Norm:
                return sizeof(std::uint16_t);
            case DataType::integer16Vector2:
            case DataType::integer16Vector2Norm:
            case DataType::unsignedInteger16Vector2:
            case DataType::unsignedInteger16Vector2Norm:
                return 2 * sizeof(std::uint16_t);
            case DataType::integer16Vector3:
            case DataType::integer16Vector3Norm:
            case DataType::unsignedInteger16Vector3:
            case DataType::unsignedInteger16Vector3Norm:
                return 3 * sizeof(std::uint16_t);
            case DataType::integer16Vector4:
            case DataType::integer16Vector4Norm:
            case DataType::unsignedInteger16Vector4:
            case DataType::unsignedInteger16Vector4Norm:
                return 4 * sizeof(std::uint16_t);

            case DataType::integer32:
            case DataType::unsignedInteger32:
                return sizeof(std::uint32_t);
            case DataType::integer32Vector2:
            case DataType::unsignedInteger32Vector2:
                return 2 * sizeof(std::uint32_t);
            case DataType::integer32Vector3:
            case DataType::unsignedInteger32Vector3:
                return 3 * sizeof(std::uint32_t);
            case DataType::integer32Vector4:
            case DataType::unsignedInteger32Vector4:
                return 4 * sizeof(std::uint32_t);

            case DataType::float32:
                return sizeof(float);
            case DataType::float32Vector2:
                return 2 * sizeof(float);
            case DataType::float32Vector3:
                return 3 * sizeof(float);
            case DataType::float32Vector4:
                return 4 * sizeof(float);
            case DataType::float32Matrix3:
                return 9 * sizeof(float);
            case DataType::float32Matrix4:
                return 16 * sizeof(float);
            default:
                return 0;
        }
    }
}

#endif // OUZEL_GRAPHICS_DATATYPE_HPP
