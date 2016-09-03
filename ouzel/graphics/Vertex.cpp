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

        VertexPC::VertexPC(Vector3 pPosition, Color pColor):
            position(pPosition), color(pColor)
        {
        }

        VertexPCT::VertexPCT()
        {
        }

        VertexPCT::VertexPCT(Vector3 pPosition, Color pColor, Vector2 pTexCoord):
            position(pPosition), color(pColor), texCoord(pTexCoord)
        {
        }
    } // namespace graphics
} // namespace ouzel
