// Ouzel by Elviss Strazdins

#ifndef OUZEL_SCENE_TEXTRENDERER_HPP
#define OUZEL_SCENE_TEXTRENDERER_HPP

#include <string>
#include "Component.hpp"
#include "../math/Color.hpp"
#include "../gui/BMFont.hpp"
#include "../graphics/BlendState.hpp"
#include "../graphics/Buffer.hpp"
#include "../graphics/Shader.hpp"
#include "../graphics/Texture.hpp"

namespace ouzel::scene
{
    class TextRenderer: public Component
    {
    public:
        TextRenderer(const std::string& fontFile,
                     float initFontSize = 1.0F,
                     const std::string& initText = std::string(),
                     math::Color initColor = math::whiteColor(),
                     const math::Vector<float, 2>& initTextAnchor = math::Vector<float, 2>{0.5F, 0.5F});

        TextRenderer(const gui::Font& initFont,
                     float initFontSize = 1.0F,
                     const std::string& initText = std::string(),
                     math::Color initColor = math::whiteColor(),
                     const math::Vector<float, 2>& initTextAnchor = math::Vector<float, 2>{0.5F, 0.5F});

        void draw(const math::Matrix<float, 4>& transformMatrix,
                  float opacity,
                  const math::Matrix<float, 4>& renderViewProjection,
                  bool wireframe) override;

        void setFont(const std::string& fontFile);

        auto getFontSize() const noexcept { return fontSize; }
        void setFontSize(float newFontSize);

        auto& getTextAnchor() const noexcept { return textAnchor; }
        void setTextAnchor(const math::Vector<float, 2>& newTextAnchor);

        auto& getText() const noexcept { return text; }
        void setText(const std::string& newText);

        auto getColor() const noexcept { return color; }
        void setColor(math::Color newColor);

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
        void updateText();

        const graphics::Shader* shader = nullptr;
        const graphics::BlendState* blendState = nullptr;

        graphics::Buffer indexBuffer;
        graphics::Buffer vertexBuffer;

        std::shared_ptr<graphics::Texture> texture;
        std::shared_ptr<graphics::Texture> whitePixelTexture;

        const gui::Font* font = nullptr;
        std::string text;
        float fontSize = 1.0F;
        math::Vector<float, 2> textAnchor{};

        std::vector<std::uint16_t> indices;
        std::vector<graphics::Vertex> vertices;

        math::Color color = math::whiteColor();

        bool needsMeshUpdate = false;
    };
}

#endif // OUZEL_SCENE_TEXTRENDERER_HPP
