// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_VERTEX_HPP
#define OUZEL_GRAPHICS_VERTEX_HPP

#include <vector>
#include "graphics/DataType.hpp"
#include "math/Vector2.hpp"
#include "math/Vector3.hpp"
#include "math/Color.hpp"

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
                    NONE,
                    BINORMAL,
                    BLEND_INDICES,
                    BLEND_WEIGHT,
                    COLOR,
                    NORMAL,
                    POSITION,
                    POSITION_TRANSFORMED,
                    POINT_SIZE,
                    TANGENT,
                    TEXTURE_COORDINATES0,
                    TEXTURE_COORDINATES1
                };

                Attribute(Usage initUsage, DataType initDataType):
                    usage(initUsage), dataType(initDataType) {}
                Usage usage = Usage::NONE;
                DataType dataType = DataType::NONE;
            };

            static const std::vector<Attribute> ATTRIBUTES;

            Vertex() {}
            Vertex(const Vector3F& initPosition, Color initColor, const Vector2F& initTexCoord, const Vector3F& initNormal):
                position(initPosition), color(initColor), normal(initNormal)
            {
                texCoords[0] = initTexCoord;
            }

            Vector3F position;
            Color color;
            Vector2F texCoords[2];
            Vector3F normal;
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_VERTEX_HPP
