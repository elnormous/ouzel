// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "Color.h"

namespace ouzel
{
    class VertexPC
    {
    public:
        Vector3 position;
        Color color;
        
        VertexPC();
        VertexPC(Vector3 position, Color color);
    };
    
    class VertexPCT
    {
    public:
        Vector3 position;
        Color color;
        Vector2 texCoord;
        
        VertexPCT();
        VertexPCT(Vector3 position, Color color, Vector2 texCoord);
    };
}
