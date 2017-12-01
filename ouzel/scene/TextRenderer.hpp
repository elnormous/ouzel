// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include "scene/Component.hpp"
#include "math/Color.hpp"
#include "gui/BMFont.hpp"
#include "graphics/BlendState.hpp"
#include "graphics/Buffer.hpp"
#include "graphics/MeshBuffer.hpp"
#include "graphics/Shader.hpp"
#include "graphics/Texture.hpp"

namespace ouzel
{
    namespace scene
    {
        class TextRenderer: public Component
        {
        public:
            static const uint32_t TYPE = Component::TEXT_RENDERER;

            TextRenderer(const std::string& fontFile,
                         bool initMipmaps = true,
                         float initFontSize = 1.0f,
                         const std::string& initText = std::string(),
                         Color initColor = Color::WHITE,
                         const Vector2& aTextAnchor = Vector2(0.5f, 0.5f));

            virtual void draw(const Matrix4& transformMatrix,
                              float opacity,
                              const Matrix4& renderViewProjection,
                              const std::shared_ptr<graphics::Texture>& renderTarget,
                              const Rectangle& renderViewport,
                              bool depthWrite,
                              bool depthTest,
                              bool wireframe,
                              bool scissorTest,
                              const Rectangle& scissorRectangle) override;

            virtual void setFont(const std::string& fontFile);

            virtual void setFontSize(float newFontSize);
            virtual float getFontSize() const { return fontSize; }

            virtual void setTextAnchor(const Vector2& newTextAnchor);
            virtual const Vector2& getTextAnchor() const { return textAnchor; }

            virtual void setText(const std::string& newText);
            virtual const std::string& getText() const { return text; }

            virtual const Color& getColor() const { return color; }
            virtual void setColor(const Color& newColor);

            virtual const std::shared_ptr<graphics::Shader>& getShader() const { return shader; }
            virtual void setShader(const std::shared_ptr<graphics::Shader>& newShader) { shader = newShader; }

            virtual const std::shared_ptr<graphics::BlendState>& getBlendState() const { return blendState; }
            virtual void setBlendState(const std::shared_ptr<graphics::BlendState>& newBlendState)  { blendState = newBlendState; }

        protected:
            void updateText();

            std::shared_ptr<graphics::Shader> shader;
            std::shared_ptr<graphics::BlendState> blendState;

            std::shared_ptr<graphics::MeshBuffer> meshBuffer;
            std::shared_ptr<graphics::Buffer> indexBuffer;
            std::shared_ptr<graphics::Buffer> vertexBuffer;

            std::shared_ptr<graphics::Texture> texture;
            std::shared_ptr<graphics::Texture> whitePixelTexture;

            std::shared_ptr<Font> font;
            std::string text;
            float fontSize = 1.0f;
            Vector2 textAnchor;

            std::vector<uint16_t> indices;
            std::vector<graphics::Vertex> vertices;

            Color color = Color::WHITE;

            bool mipmaps = true;
            bool needsMeshUpdate = false;
        };
    } // namespace scene
} // namespace ouzel
