// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Vertex.h"

namespace ouzel
{
    namespace video
    {
        VertexPC::VertexPC()
        {

        }

        VertexPC::VertexPC(Vector3 position, Color color):
            position(position), color(color)
        {

        }

        VertexPCT::VertexPCT()
        {

        }

        VertexPCT::VertexPCT(Vector3 position, Color color, Vector2 texCoord):
            position(position), color(color), texCoord(texCoord)
        {

        }
    } // namespace video
} // namespace ouzel
