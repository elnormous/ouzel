// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Vertex.h"

namespace ouzel
{
    namespace graphics
    {
        VertexPC::VertexPC()
        {
        }

        VertexPC::VertexPC(const Vector3& aPosition, Color aColor):
            position(aPosition), color(aColor)
        {
        }

        VertexPCT::VertexPCT()
        {
        }

        VertexPCT::VertexPCT(const Vector3& aPosition, Color aColor, const Vector2& aTexCoord):
            position(aPosition), color(aColor), texCoord(aTexCoord)
        {
        }
    } // namespace graphics
} // namespace ouzel
