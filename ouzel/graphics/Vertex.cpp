// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Vertex.hpp"

namespace ouzel
{
    namespace graphics
    {
        const std::vector<Vertex::Attribute> Vertex::ATTRIBUTES = {
            Vertex::Attribute(Vertex::Attribute::Usage::POSITION, DataType::FLOAT_VECTOR3),
            Vertex::Attribute(Vertex::Attribute::Usage::COLOR, DataType::UNSIGNED_BYTE_VECTOR4_NORM),
            Vertex::Attribute(Vertex::Attribute::Usage::TEXTURE_COORDINATES0, DataType::FLOAT_VECTOR2),
            Vertex::Attribute(Vertex::Attribute::Usage::TEXTURE_COORDINATES1, DataType::FLOAT_VECTOR2),
            Vertex::Attribute(Vertex::Attribute::Usage::NORMAL, DataType::FLOAT_VECTOR3)
        };

        Vertex::Vertex()
        {
        }

        Vertex::Vertex(const Vector3& initPosition, Color initColor, const Vector2& initTexCoord, const Vector3& initNormal):
            position(initPosition), color(initColor), normal(initNormal)
        {
            texCoords[0] = initTexCoord;
        }
    } // namespace graphics
} // namespace ouzel
