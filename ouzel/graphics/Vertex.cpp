// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Vertex.hpp"

namespace ouzel
{
    namespace graphics
    {
        const std::vector<VertexAttribute> VertexPC::ATTRIBUTES = {
            VertexAttribute(VertexAttribute::Usage::POSITION, 0, DataType::FLOAT_VECTOR3, false),
            VertexAttribute(VertexAttribute::Usage::COLOR, 0, DataType::UNSIGNED_BYTE_VECTOR4, true)
        };

        VertexPC::VertexPC()
        {
        }

        VertexPC::VertexPC(const Vector3& aPosition, Color aColor):
            position(aPosition), color(aColor)
        {
        }

        const std::vector<VertexAttribute> VertexPCT::ATTRIBUTES = {
            VertexAttribute(VertexAttribute::Usage::POSITION, 0, DataType::FLOAT_VECTOR3, false),
            VertexAttribute(VertexAttribute::Usage::COLOR, 0, DataType::UNSIGNED_BYTE_VECTOR4, true),
            VertexAttribute(VertexAttribute::Usage::TEXTURE_COORDINATES, 0, DataType::FLOAT_VECTOR2, false)
        };

        VertexPCT::VertexPCT()
        {
        }

        VertexPCT::VertexPCT(const Vector3& aPosition, Color aColor, const Vector2& aTexCoord):
            position(aPosition), color(aColor), texCoord(aTexCoord)
        {
        }

        const std::vector<VertexAttribute> VertexPCTN::ATTRIBUTES = {
            VertexAttribute(VertexAttribute::Usage::POSITION, 0, DataType::FLOAT_VECTOR3, false),
            VertexAttribute(VertexAttribute::Usage::COLOR, 0, DataType::UNSIGNED_BYTE_VECTOR4, true),
            VertexAttribute(VertexAttribute::Usage::TEXTURE_COORDINATES, 0, DataType::FLOAT_VECTOR2, false),
            VertexAttribute(VertexAttribute::Usage::NORMAL, 0, DataType::FLOAT_VECTOR3, false)
        };

        VertexPCTN::VertexPCTN()
        {
        }

        VertexPCTN::VertexPCTN(const Vector3& aPosition, Color aColor, const Vector2& aTexCoord, const Vector3& aNormal):
            position(aPosition), color(aColor), texCoord(aTexCoord), normal(aNormal)
        {
        }
    } // namespace graphics
} // namespace ouzel
