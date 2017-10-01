// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <vector>
#include "graphics/DataType.hpp"
#include "math/Vector2.hpp"
#include "math/Vector3.hpp"
#include "math/Color.hpp"

namespace ouzel
{
    namespace graphics
    {
        struct VertexAttribute
        {
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
                TEXTURE_COORDINATES
            };

            VertexAttribute(Usage aUsage, uint32_t aIndex, DataType aDataType, bool aNormalized):
                usage(aUsage), index(aIndex), dataType(aDataType), normalized(aNormalized) {}
            Usage usage = Usage::NONE;
            uint32_t index = 0;
            DataType dataType = DataType::NONE;
            bool normalized = false;
        };

        const uint32_t VERTEX_ATTRIBUTE_COUNT = 5;

        class VertexPC
        {
        public:
            static const std::vector<VertexAttribute> ATTRIBUTES;

            Vector3 position;
            Color color;

            VertexPC();
            VertexPC(const Vector3& aPosition, Color aColor);
        };

        class VertexPCT
        {
        public:
            static const std::vector<VertexAttribute> ATTRIBUTES;

            Vector3 position;
            Color color;
            Vector2 texCoord;

            VertexPCT();
            VertexPCT(const Vector3& aPosition, Color aColor, const Vector2& aTexCoord);
        };

        class VertexPCTN
        {
        public:
            static const std::vector<VertexAttribute> ATTRIBUTES;

            Vector3 position;
            Color color;
            Vector2 texCoord;
            Vector3 normal;

            VertexPCTN();
            VertexPCTN(const Vector3& aPosition, Color aColor, const Vector2& aTexCoord, const Vector3& aNormal);
        };
    } // namespace graphics
} // namespace ouzel
