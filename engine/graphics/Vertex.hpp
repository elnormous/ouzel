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
                textureCoordinates
            };

            using Index = std::size_t;

            struct Semantic final
            {
                constexpr Semantic(Usage initUsage, Index initIndex = 0) noexcept:
                    usage{initUsage},
                    index{initIndex}
                {}

                constexpr bool operator<(const Semantic& other) const noexcept
                {
                    return usage == other.usage ?
                        index < other.index :
                        usage < other.usage;
                }

                Usage usage;
                Index index;
            };

            constexpr Attribute(Semantic initSemantic, DataType initDataType) noexcept:
                semantic{initSemantic}, dataType{initDataType} {}
            Semantic semantic;
            DataType dataType;
        };

        constexpr Vertex() noexcept = default;
        constexpr Vertex(const math::Vector<float, 3>& initPosition, math::Color initColor,
                         const math::Vector<float, 2>& initTexCoord, const math::Vector<float, 3>& initNormal) noexcept:
            position{initPosition}, color{initColor},
            texCoords{initTexCoord}, normal{initNormal}
        {
        }

        math::Vector<float, 3> position{};
        math::Color color{};
        std::array<math::Vector<float, 2>, 2> texCoords{};
        math::Vector<float, 3> normal{};
    };
}

#endif // OUZEL_GRAPHICS_VERTEX_HPP
