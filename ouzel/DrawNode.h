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

            void point(const Vector2& position, const video::Color& color);
            void line(const Vector2& start, const Vector2& finish, const video::Color& color);
            void circle(const Vector2& position, float radius, const video::Color& color, bool fill = false, uint32_t segments = 20);
            void rectangle(const Rectangle& rectangle, const video::Color& color, bool fill = false);
            void triangle(const Vector2 (&positions)[3], const video::Color& color, bool fill = false);

        protected:
            struct DrawCommand
            {
                video::Renderer::DrawMode mode;
                video::MeshBufferPtr mesh;
            };

            video::ShaderPtr _shader;
            std::vector<DrawCommand> _drawCommands;

            uint32_t _uniModelViewProj;
        };
    } // namespace scene
} // namespace ouzel
