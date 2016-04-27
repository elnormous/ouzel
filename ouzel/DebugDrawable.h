// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "Types.h"
#include "Drawable.h"
#include "Renderer.h"
#include "Color.h"

namespace ouzel
{
    namespace scene
    {
        class DebugDrawable: public Drawable
        {
        public:
            DebugDrawable();

            virtual void draw(const Matrix4& projectionMatrix, const Matrix4& transformMatrix, const graphics::Color& color) override;

            void clear();

            void point(const Vector2& position, const graphics::Color& color);
            void line(const Vector2& start, const Vector2& finish, const graphics::Color& color);
            void circle(const Vector2& position, float radius, const graphics::Color& color, bool fill = false, uint32_t segments = 20);
            void rectangle(const Rectangle& rectangle, const graphics::Color& color, bool fill = false);
            void triangle(const Vector2 (&positions)[3], const graphics::Color& color, bool fill = false);

        protected:
            struct DrawCommand
            {
                graphics::Renderer::DrawMode mode;
                graphics::MeshBufferPtr mesh;
            };

            graphics::ShaderPtr shader;
            std::vector<DrawCommand> drawCommands;
        };
    } // namespace scene
} // namespace ouzel
