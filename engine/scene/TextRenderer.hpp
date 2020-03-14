// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_SCENE_TEXTRENDERER_HPP
#define OUZEL_SCENE_TEXTRENDERER_HPP

#include <string>
#include "scene/Component.hpp"
#include "math/Color.hpp"
#include "gui/BMFont.hpp"
#include "graphics/BlendState.hpp"
#include "graphics/Buffer.hpp"
#include "graphics/Shader.hpp"
#include "graphics/Texture.hpp"

namespace ouzel
{
    namespace scene
    {
        class TextRenderer: public Component
        {
        public:
            TextRenderer(const std::string& fontFile,
                         float initFontSize = 1.0F,
                         const std::string& initText = std::string(),
                         Color initColor = Color::white(),
                         const Vector2F& initTextAnchor = Vector2F{0.5F, 0.5F});

            void draw(const Matrix4F& transformMatrix,
                      float opacity,
                      const Matrix4F& renderViewProjection,
                      bool wireframe) override;

            void setFont(const std::string& fontFile);

            inline auto getFontSize() const noexcept { return fontSize; }
            void setFontSize(float newFontSize);

            inline auto& getTextAnchor() const noexcept { return textAnchor; }
            void setTextAnchor(const Vector2F& newTextAnchor);

            inline auto& getText() const noexcept { return text; }
            void setText(const std::string& newText);

            inline auto getColor() const noexcept { return color; }
            void setColor(Color newColor);

            inline auto& getShader() const noexcept { return shader; }
            inline void setShader(const graphics::Shader* newShader)
            {
                shader = newShader;
            }

            inline auto& getBlendState() const noexcept { return blendState; }
            inline void setBlendState(const graphics::BlendState* newBlendState)
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
            Vector2F textAnchor;

            std::vector<std::uint16_t> indices;
            std::vector<graphics::Vertex> vertices;

            Color color = Color::white();

            bool needsMeshUpdate = false;
        };
    } // namespace scene
} // namespace ouzel

#endif // OUZEL_SCENE_TEXTRENDERER_HPP
