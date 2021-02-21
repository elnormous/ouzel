// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_VERTEX_HPP
#define OUZEL_GRAPHICS_VERTEX_HPP

#include <array>
#include "DataType.hpp"
#include "../math/Vector.hpp"
#include "../math/Color.hpp"

namespace ouzel::graphics
{
    class Vertex final
    {
    public:
        class Attribute final
        {
        public:
            enum class Usage
            {
                binormal,
                blendIndices,
                blendWeight,
                color,
                normal,
                position,
                positionTransformed,
                pointSize,
                tangent,
                textureCoordinates0,
                textureCoordinates1
            };

            constexpr Attribute(Usage initUsage, DataType initDataType) noexcept:
                usage(initUsage), dataType(initDataType) {}
            Usage usage;
            DataType dataType;
        };

        constexpr Vertex() noexcept = default;
        constexpr Vertex(const Vector3F& initPosition, Color initColor,
                         const Vector2F& initTexCoord, const Vector3F& initNormal) noexcept:
            position(initPosition), color(initColor),
            texCoords{initTexCoord}, normal(initNormal)
        {
        }

        Vector3F position;
        Color color;
        std::array<Vector2F, 2> texCoords;
        Vector3F normal;
    };
}

#endif // OUZEL_GRAPHICS_VERTEX_HPP
