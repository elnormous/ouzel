// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "Types.h"
#include "Rectangle.h"
#include "Vertex.h"

namespace ouzel
{
    namespace scene
    {
        class SpriteFrame
        {
        public:
            Rectangle rectangle;
            std::vector<graphics::VertexPCT> vertices;
            graphics::MeshBufferPtr meshBuffer;
            graphics::TexturePtr texture;
        };
    } // scene
} // ouzel
