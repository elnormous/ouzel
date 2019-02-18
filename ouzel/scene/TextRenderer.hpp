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
            static constexpr uint32_t CLASS = Component::TEXT_RENDERER;

            TextRenderer(const std::string& fontFile,
                         float initFontSize = 1.0F,
                         const std::string& initText = std::string(),
                         Color initColor = Color::WHITE,
                         const Vector2F& initTextAnchor = Vector2F(0.5F, 0.5F));

            void draw(const Matrix4F& transformMatrix,
                      float opacity,
                      const Matrix4F& renderViewProjection,
                      bool wireframe) override;

            void setFont(const std::string& fontFile);

            inline float getFontSize() const { return fontSize; }
            void setFontSize(float newFontSize);

            inline const Vector2F& getTextAnchor() const { return textAnchor; }
            void setTextAnchor(const Vector2F& newTextAnchor);

            inline const std::string& getText() const { return text; }
            void setText(const std::string& newText);

            inline Color getColor() const { return color; }
            void setColor(Color newColor);

            inline graphics::Shader* getShader() const { return shader; }
            inline void setShader(graphics::Shader* newShader) { shader = newShader; }

            inline graphics::BlendState* getBlendState() const { return blendState; }
            inline void setBlendState(graphics::BlendState* newBlendState) { blendState = newBlendState; }

        private:
            void updateText();

            graphics::Shader* shader = nullptr;
            graphics::BlendState* blendState = nullptr;

            std::unique_ptr<graphics::Buffer> indexBuffer;
            std::unique_ptr<graphics::Buffer> vertexBuffer;

            std::unique_ptr<graphics::Texture> ownedTexture;
            graphics::Texture* texture = nullptr;
            graphics::Texture* whitePixelTexture = nullptr;

            Font* font = nullptr;
            std::string text;
            float fontSize = 1.0F;
            Vector2F textAnchor;

            std::vector<uint16_t> indices;
            std::vector<graphics::Vertex> vertices;

            Color color = Color::WHITE;

            bool needsMeshUpdate = false;
        };
    } // namespace scene
} // namespace ouzel

#endif // OUZEL_SCENE_TEXTRENDERER_HPP
