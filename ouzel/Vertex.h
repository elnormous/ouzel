// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Vector2.h"
#include "Vector4.h"
#include "Color.h"

namespace ouzel
{
    class Vertex
    {
    public:
        Vector4 position;
        Color color;
        Vector2 texCoord;
        
        Vertex();
        Vertex(Vector4 position, Color color, Vector2 texCoord);
    };
}
