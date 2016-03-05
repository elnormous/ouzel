// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "Types.h"
#include "Node.h"
#include "Renderer.h"
#include "Color.h"

namespace ouzel
{
namespace scene
{
    class DrawNode: public Node
    {
    public:
        DrawNode();
        
        virtual void draw();
        
        void clear();
        
        void point(const Vector2& position, const Color& color);
        void line(const Vector2& start, const Vector2& finish, const Color& color);
        void circle(const Vector2& position, float radius, const Color& color, bool fill = false, uint32_t segments = 20);
        void rectangle(const Rectangle& rectangle, const Color& color, bool fill = false);
        
    protected:
        struct DrawCommand
        {
            Renderer::DrawMode mode;
            MeshBufferPtr mesh;
        };
        
        ShaderPtr _shader;
        std::vector<DrawCommand> _drawCommands;
        
        uint32_t _uniModelViewProj;
    };
} // namespace scene
} // namespace ouzel
