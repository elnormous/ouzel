// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "Color.h"

namespace ouzel
{
    class Vertex
    {
    public:
        Vector3 position;
        Color color;
        Vector2 texCoord;
        
        Vertex();
        Vertex(Vector3 position, Color color, Vector2 texCoord);
    };
}
