// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "scene/Component.h"
#include "graphics/Renderer.h"
#include "math/Color.h"
#include "graphics/BlendState.h"
#include "graphics/Buffer.h"
#include "graphics/MeshBuffer.h"
#include "graphics/Shader.h"

namespace ouzel
{
    namespace scene
    {
        class ShapeDrawable: public Component
        {
        public:
            ShapeDrawable();

            virtual void draw(const Matrix4& transformMatrix,
                              const Color& drawColor,
                              scene::Camera* camera) override;

            virtual void drawWireframe(const Matrix4& transformMatrix,
                                       const Color& drawColor,
                                       scene::Camera* camera) override;

            void clear();

            void point(const Vector2& position, const Color& color);
            void line(const Vector2& start, const Vector2& finish, const Color& color);
            void circle(const Vector2& position, float radius, const Color& color, bool fill = false, uint32_t segments = 20);
            void rectangle(const Rectangle& rectangle, const Color& color, bool fill = false);
            void triangle(const Vector2 (&positions)[3], const Color& color, bool fill = false);
            void polygon(const std::vector<Vector2>& edges, const Color& color, bool fill = false);

            virtual const std::shared_ptr<graphics::Shader>& getShader() const { return shader; }
            virtual void setShader(const std::shared_ptr<graphics::Shader>& newShader) { shader = newShader; }

            virtual const std::shared_ptr<graphics::BlendState>& getBlendState() const { return blendState; }
            virtual void setBlendState(const std::shared_ptr<graphics::BlendState>& newBlendState)  { blendState = newBlendState; }

        protected:
            struct DrawCommand
            {
                graphics::Renderer::DrawMode mode;
                uint32_t indexCount;
                uint32_t startIndex;
            };

            std::shared_ptr<graphics::Shader> shader;
            std::shared_ptr<graphics::BlendState> blendState;
            std::shared_ptr<graphics::MeshBuffer> meshBuffer;
            std::shared_ptr<graphics::Buffer> indexBuffer;
            std::shared_ptr<graphics::Buffer> vertexBuffer;

            std::vector<DrawCommand> drawCommands;

            std::vector<uint16_t> indices;
            std::vector<ouzel::graphics::VertexPC> vertices;
            bool dirty = false;
        };
    } // namespace scene
} // namespace ouzel
