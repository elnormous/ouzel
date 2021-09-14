// Ouzel by Elviss Strazdins

#ifndef OUZEL_SCENE_SHAPERENDERER_HPP
#define OUZEL_SCENE_SHAPERENDERER_HPP

#include <vector>
#include "Component.hpp"
#include "../graphics/Graphics.hpp"
#include "../graphics/BlendState.hpp"
#include "../graphics/Buffer.hpp"
#include "../graphics/Shader.hpp"
#include "../math/Color.hpp"

namespace ouzel::scene
{
    class ShapeRenderer: public Component
    {
    public:
        ShapeRenderer();

        void draw(const math::Matrix<float, 4>& transformMatrix,
                  float opacity,
                  const math::Matrix<float, 4>& renderViewProjection,
                  bool wireframe) override;

        void clear();

        void line(const math::Vector<float, 2>& start,
                  const math::Vector<float, 2>& finish,
                  math::Color color,
                  float thickness = 0.0F);

        void circle(const math::Vector<float, 2>& position,
                    float radius,
                    math::Color color,
                    bool fill = false,
                    std::uint32_t segments = 16,
                    float thickness = 0.0F);

        void rectangle(const math::Rect<float>& rectangle,
                       math::Color color,
                       bool fill = false,
                       float thickness = 0.0F);

        void polygon(const std::vector<math::Vector<float, 2>>& edges,
                     math::Color color,
                     bool fill = false,
                     float thickness = 0.0F);

        void curve(const std::vector<math::Vector<float, 2>>& controlPoints,
                   math::Color color,
                   std::uint32_t segments = 16,
                   float thickness = 0.0F);

        auto& getShader() const noexcept { return shader; }
        void setShader(const graphics::Shader* newShader)
        {
            shader = newShader;
        }

        auto& getBlendState() const noexcept { return blendState; }
        void setBlendState(const graphics::BlendState* newBlendState)
        {
            blendState = newBlendState;
        }

    private:
        struct DrawCommand final
        {
            graphics::DrawMode mode;
            std::uint32_t indexCount;
            std::uint32_t startIndex;
        };

        const graphics::Shader* shader = nullptr;
        const graphics::BlendState* blendState = nullptr;
        graphics::Buffer indexBuffer;
        graphics::Buffer vertexBuffer;

        std::vector<DrawCommand> drawCommands;

        std::vector<std::uint16_t> indices;
        std::vector<graphics::Vertex> vertices;
        bool dirty = false;
    };
}

#endif // OUZEL_SCENE_SHAPERENDERER_HPP
