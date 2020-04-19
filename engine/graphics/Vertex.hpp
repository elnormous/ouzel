// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_VERTEX_HPP
#define OUZEL_GRAPHICS_VERTEX_HPP

#include "DataType.hpp"
#include "../math/Vector.hpp"
#include "../math/Color.hpp"

namespace ouzel
{
    namespace graphics
    {
        class Vertex final
        {
        public:
            class Attribute final
            {
            public:
                enum class Usage
                {
                    Binormal,
                    BlendIndices,
                    BlendWeight,
                    Color,
                    Normal,
                    Position,
                    PositionTransformed,
                    PointSize,
                    Tangent,
                    TextureCoordinates0,
                    TextureCoordinates1
                };

                Attribute(Usage initUsage, DataType initDataType) noexcept:
                    usage(initUsage), dataType(initDataType) {}
                Usage usage;
                DataType dataType;
            };

            Vertex() noexcept = default;
            Vertex(const Vector3F& initPosition, Color initColor,
                   const Vector2F& initTexCoord, const Vector3F& initNormal) noexcept:
                position(initPosition), color(initColor),
                texCoords{initTexCoord}, normal(initNormal)
            {
            }

            Vector3F position;
            Color color;
            Vector2F texCoords[2];
            Vector3F normal;
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_VERTEX_HPP
