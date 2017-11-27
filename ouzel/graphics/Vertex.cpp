// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Vertex.hpp"

namespace ouzel
{
    namespace graphics
    {
        const std::vector<Vertex::Attribute> Vertex::ATTRIBUTES = {
            Vertex::Attribute(Vertex::Attribute::Usage::POSITION, DataType::FLOAT_VECTOR3, false),
            Vertex::Attribute(Vertex::Attribute::Usage::COLOR, DataType::UNSIGNED_BYTE_VECTOR4, true),
            Vertex::Attribute(Vertex::Attribute::Usage::TEXTURE_COORDINATES0, DataType::FLOAT_VECTOR2, false),
            Vertex::Attribute(Vertex::Attribute::Usage::TEXTURE_COORDINATES1, DataType::FLOAT_VECTOR2, false),
            Vertex::Attribute(Vertex::Attribute::Usage::NORMAL, DataType::FLOAT_VECTOR3, false)
        };

        Vertex::Vertex()
        {
        }

        Vertex::Vertex(const Vector3& aPosition, Color aColor, const Vector2& aTexCoord, const Vector3& aNormal):
            position(aPosition), color(aColor), normal(aNormal)
        {
            texCoords[0] = aTexCoord;
        }

        const std::vector<Vertex::Attribute> VertexPC::ATTRIBUTES = {
            Vertex::Attribute(Vertex::Attribute::Usage::POSITION, DataType::FLOAT_VECTOR3, false),
            Vertex::Attribute(Vertex::Attribute::Usage::COLOR, DataType::UNSIGNED_BYTE_VECTOR4, true)
        };

        VertexPC::VertexPC()
        {
        }

        VertexPC::VertexPC(const Vector3& aPosition, Color aColor):
            position(aPosition), color(aColor)
        {
        }

        const std::vector<Vertex::Attribute> VertexPCT::ATTRIBUTES = {
            Vertex::Attribute(Vertex::Attribute::Usage::POSITION, DataType::FLOAT_VECTOR3, false),
            Vertex::Attribute(Vertex::Attribute::Usage::COLOR, DataType::UNSIGNED_BYTE_VECTOR4, true),
            Vertex::Attribute(Vertex::Attribute::Usage::TEXTURE_COORDINATES0, DataType::FLOAT_VECTOR2, false)
        };

        VertexPCT::VertexPCT()
        {
        }

        VertexPCT::VertexPCT(const Vector3& aPosition, Color aColor, const Vector2& aTexCoord):
            position(aPosition), color(aColor), texCoord(aTexCoord)
        {
        }

        const std::vector<Vertex::Attribute> VertexPCTN::ATTRIBUTES = {
            Vertex::Attribute(Vertex::Attribute::Usage::POSITION, DataType::FLOAT_VECTOR3, false),
            Vertex::Attribute(Vertex::Attribute::Usage::COLOR, DataType::UNSIGNED_BYTE_VECTOR4, true),
            Vertex::Attribute(Vertex::Attribute::Usage::TEXTURE_COORDINATES0, DataType::FLOAT_VECTOR2, false),
            Vertex::Attribute(Vertex::Attribute::Usage::NORMAL, DataType::FLOAT_VECTOR3, false)
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
