// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include "scene/Component.h"
#include "utils/Types.h"
#include "graphics/Color.h"
#include "gui/BMFont.h"

namespace ouzel
{
    namespace scene
    {
        class TextDrawable: public Component
        {
        public:
            TextDrawable(const std::string& fontFile, const std::string& pText, const Vector2& pTextAnchor = Vector2(0.5f, 0.5f));
            virtual ~TextDrawable();

            virtual void draw(const Matrix4& viewProjectionMatrix,
                              const Matrix4& transformMatrix,
                              const graphics::Color& drawColor,
                              const graphics::RenderTargetPtr& renderTarget) override;

            virtual void drawWireframe(const Matrix4& viewProjectionMatrix,
                                       const Matrix4& transformMatrix,
                                       const graphics::Color& drawColor,
                                       const graphics::RenderTargetPtr& renderTarget) override;

            virtual void setFont(const std::string& fontFile);

            virtual void setTextAnchor(const Vector2& newTextAnchor);
            virtual const Vector2& getTextAnchor() const { return textAnchor; }

            virtual void setText(const std::string& newText);
            virtual const std::string& getText() const { return text; }

            virtual const graphics::Color& getColor() const { return color; }
            virtual void setColor(const graphics::Color& newColor);

            virtual const graphics::ShaderPtr& getShader() const { return shader; }
            virtual void setShader(const graphics::ShaderPtr& newShader) { shader = newShader; }

            virtual const graphics::BlendStatePtr& getBlendState() const { return blendState; }
            virtual void setBlendState(const graphics::BlendStatePtr& newBlendState)  { blendState = newBlendState; }

        protected:
            void updateBounds();

            graphics::ShaderPtr shader;
            graphics::BlendStatePtr blendState;

            graphics::MeshBufferPtr meshBuffer;
            graphics::IndexBufferPtr indexBuffer;
            graphics::VertexBufferPtr vertexBuffer;

            graphics::TexturePtr texture;
            graphics::TexturePtr whitePixelTexture;

            BMFont font;
            Vector2 textAnchor;
            std::string text;

            std::vector<uint16_t> indices;
            std::vector<graphics::VertexPCT> vertices;

            graphics::Color color = graphics::Color(255, 255, 255, 255);

            bool needsMeshUpdate = false;
        };
    } // namespace scene
} // namespace ouzel
