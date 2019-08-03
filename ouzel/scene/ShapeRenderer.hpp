// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_SCENE_SHAPERENDERER_HPP
#define OUZEL_SCENE_SHAPERENDERER_HPP

#include <vector>
#include "scene/Component.hpp"
#include "graphics/Renderer.hpp"
#include "math/Color.hpp"
#include "graphics/BlendState.hpp"
#include "graphics/Buffer.hpp"
#include "graphics/Shader.hpp"

namespace ouzel
{
    namespace scene
    {
        class ShapeRenderer: public Component
        {
        public:
            ShapeRenderer();

            void draw(const Matrix4F& transformMatrix,
                      float opacity,
                      const Matrix4F& renderViewProjection,
                      bool wireframe) override;

            void clear();

            void line(const Vector2F& start,
                      const Vector2F& finish,
                      Color color,
                      float thickness = 0.0F);

            void circle(const Vector2F& position,
                        float radius,
                        Color color,
                        bool fill = false,
                        uint32_t segments = 16,
                        float thickness = 0.0F);

            void rectangle(const RectF& rectangle,
                           Color color,
                           bool fill = false,
                           float thickness = 0.0F);

            void polygon(const std::vector<Vector2F>& edges,
                         Color color,
                         bool fill = false,
                         float thickness = 0.0F);

            void curve(const std::vector<Vector2F>& controlPoints,
                       Color color,
                       uint32_t segments = 16,
                       float thickness = 0.0F);

            inline auto& getShader() const noexcept { return shader; }
            inline void setShader(const std::shared_ptr<graphics::Shader>& newShader) { shader = newShader; }

            inline auto& getBlendState() const noexcept { return blendState; }
            inline void setBlendState(const std::shared_ptr<graphics::BlendState>& newBlendState) { blendState = newBlendState; }

        private:
            struct DrawCommand final
            {
                graphics::DrawMode mode;
                uint32_t indexCount;
                uint32_t startIndex;
            };

            std::shared_ptr<graphics::Shader> shader;
            std::shared_ptr<graphics::BlendState> blendState;
            graphics::Buffer indexBuffer;
            graphics::Buffer vertexBuffer;

            std::vector<DrawCommand> drawCommands;

            std::vector<uint16_t> indices;
            std::vector<graphics::Vertex> vertices;
            bool dirty = false;
        };
    } // namespace scene
} // namespace ouzel

#endif // OUZEL_SCENE_SHAPERENDERER_HPP
