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
                         bool aMipmaps = true,
                         const std::string& aText = std::string(),
                         Color aColor = Color::WHITE,
                         const Vector2& aTextAnchor = Vector2(0.5f, 0.5f));

            virtual void draw(const Matrix4& transformMatrix,
                              const Color& drawColor,
                              const Matrix4& renderViewProjection,
                              const std::shared_ptr<graphics::Texture>& renderTarget,
                              const Rectangle& renderViewport,
                              bool depthWrite,
                              bool depthTest,
                              bool wireframe,
                              bool scissorTest,
                              const Rectangle& scissorRectangle) override;

            virtual void setFont(const std::string& fontFile);

            virtual void setTextAnchor(const Vector2& newTextAnchor);
            virtual const Vector2& getTextAnchor() const { return textAnchor; }

            virtual void setTextScale(const Vector2& newTextScale);
            virtual const Vector2& getTextScale() const { return textScale; }

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

            BMFont font;
            std::string text;
            Vector2 textAnchor;
            Vector2 textScale = Vector2(1.0f, 1.0f);

            std::vector<uint16_t> indices;
            std::vector<graphics::VertexPCT> vertices;

            Color color = Color::WHITE;

            bool mipmaps = true;
            bool needsMeshUpdate = false;
        };
    } // namespace scene
} // namespace ouzel
