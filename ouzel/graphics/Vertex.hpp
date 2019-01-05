// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

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
            class Attribute
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
            Vertex(const Vector3<float>& initPosition, Color initColor, const Vector2<float>& initTexCoord, const Vector3<float>& initNormal):
                position(initPosition), color(initColor), normal(initNormal)
            {
                texCoords[0] = initTexCoord;
            }

            Vector3<float> position;
            Color color;
            Vector2<float> texCoords[2];
            Vector3<float> normal;
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_VERTEX_HPP
