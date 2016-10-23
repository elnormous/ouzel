// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "utils/Types.h"
#include "scene/Component.h"
#include "graphics/Renderer.h"
#include "graphics/Color.h"

namespace ouzel
{
    namespace scene
    {
        class ShapeDrawable: public Component
        {
        public:
            ShapeDrawable();

            virtual void draw(const Matrix4& transformMatrix,
                              const graphics::Color& drawColor,
                              scene::Camera* camera) override;

            virtual void drawWireframe(const Matrix4& transformMatrix,
                                       const graphics::Color& drawColor,
                                       scene::Camera* camera) override;

            void clear();

            void point(const Vector2& position, const graphics::Color& color);
            void line(const Vector2& start, const Vector2& finish, const graphics::Color& color);
            void circle(const Vector2& position, float radius, const graphics::Color& color, bool fill = false, uint32_t segments = 20);
            void rectangle(const Rectangle& rectangle, const graphics::Color& color, bool fill = false);
            void triangle(const Vector2 (&positions)[3], const graphics::Color& color, bool fill = false);

            virtual const graphics::ShaderPtr& getShader() const { return shader; }
            virtual void setShader(const graphics::ShaderPtr& newShader) { shader = newShader; }

            virtual const graphics::BlendStatePtr& getBlendState() const { return blendState; }
            virtual void setBlendState(const graphics::BlendStatePtr& newBlendState)  { blendState = newBlendState; }

        protected:
            struct DrawCommand
            {
                graphics::Renderer::DrawMode mode;
                uint32_t indexCount;
                uint32_t startIndex;
            };

            graphics::ShaderPtr shader;
            graphics::BlendStatePtr blendState;
            graphics::MeshBufferPtr meshBuffer;
            graphics::IndexBufferPtr indexBuffer;
            graphics::VertexBufferPtr vertexBuffer;

            std::vector<DrawCommand> drawCommands;

            std::vector<uint16_t> indices;
            std::vector<ouzel::graphics::VertexPC> vertices;
        };
    } // namespace scene
} // namespace ouzel
