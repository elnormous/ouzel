// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

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
                         const Vector2<float>& initTextAnchor = Vector2<float>(0.5F, 0.5F));

            void draw(const Matrix4<float>& transformMatrix,
                      float opacity,
                      const Matrix4<float>& renderViewProjection,
                      bool wireframe) override;

            void setFont(const std::string& fontFile);

            inline float getFontSize() const { return fontSize; }
            void setFontSize(float newFontSize);

            inline const Vector2<float>& getTextAnchor() const { return textAnchor; }
            void setTextAnchor(const Vector2<float>& newTextAnchor);

            inline const std::string& getText() const { return text; }
            void setText(const std::string& newText);

            inline Color getColor() const { return color; }
            void setColor(Color newColor);

            inline const std::shared_ptr<graphics::Shader>& getShader() const { return shader; }
            inline void setShader(const std::shared_ptr<graphics::Shader>& newShader) { shader = newShader; }

            inline const std::shared_ptr<graphics::BlendState>& getBlendState() const { return blendState; }
            inline void setBlendState(const std::shared_ptr<graphics::BlendState>& newBlendState) { blendState = newBlendState; }

        private:
            void updateText();

            std::shared_ptr<graphics::Shader> shader;
            std::shared_ptr<graphics::BlendState> blendState;

            std::shared_ptr<graphics::Buffer> indexBuffer;
            std::shared_ptr<graphics::Buffer> vertexBuffer;

            std::shared_ptr<graphics::Texture> texture;
            std::shared_ptr<graphics::Texture> whitePixelTexture;

            std::shared_ptr<Font> font;
            std::string text;
            float fontSize = 1.0F;
            Vector2<float> textAnchor;

            std::vector<uint16_t> indices;
            std::vector<graphics::Vertex> vertices;

            Color color = Color::WHITE;

            bool needsMeshUpdate = false;
        };
    } // namespace scene
} // namespace ouzel

#endif // OUZEL_SCENE_TEXTRENDERER_HPP
