// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "math/Vector2.h"
#include "math/Vector3.h"
#include "graphics/Color.h"

namespace ouzel
{
    namespace graphics
    {
        enum VertexAttributes
        {
            VERTEX_POSITION = 0x01,
            VERTEX_COLOR = 0x02,
            VERTEX_NORMAL = 0x04,
            VERTEX_TEXCOORD0 = 0x08,
            VERTEX_TEXCOORD1 = 0x10
        };

        const uint32_t VERTEX_ATTRIBUTE_COUNT = 5;

        class VertexPC
        {
        public:
            static const uint32_t ATTRIBUTES = VERTEX_POSITION | VERTEX_COLOR;

            Vector3 position;
            Color color;

            VertexPC();
            VertexPC(const Vector3& aPosition, Color aColor);
        };

        class VertexPCT
        {
        public:
            static const uint32_t ATTRIBUTES = VERTEX_POSITION | VERTEX_COLOR | VERTEX_TEXCOORD0;

            Vector3 position;
            Color color;
            Vector2 texCoord;

            VertexPCT();
            VertexPCT(const Vector3& aPosition, Color aColor, const Vector2& aTexCoord);
        };
    } // namespace graphics
} // namespace ouzel
