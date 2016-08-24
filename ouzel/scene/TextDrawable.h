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
            TextDrawable();
            TextDrawable(const std::string& fontFile, const std::string& pText, const Vector2& pTextAnchor = Vector2(0.5f, 0.5f));
            virtual ~TextDrawable();

            bool init(const std::string& fontFile, const std::string& newText, const Vector2& newTextAnchor = Vector2(0.5f, 0.5f));

            virtual void draw(const Matrix4& projectionMatrix,
                              const Matrix4& transformMatrix,
                              const graphics::Color& drawColor,
                              const graphics::RenderTargetPtr& renderTarget) override;

            virtual void setText(const std::string& newText);
            virtual const std::string& getText() const { return text; }

            virtual const graphics::Color& getColor() const { return color; }
            virtual void setColor(const graphics::Color& newColor);

        protected:
            void updateMesh();

            graphics::TexturePtr texture;
            graphics::MeshBufferPtr meshBuffer;
            graphics::ShaderPtr shader;
            graphics::BlendStatePtr blendState;

            BMFont font;
            Vector2 textAnchor;
            std::string text;
            uint32_t indexCount = 0;

            graphics::Color color = graphics::Color(255, 255, 255, 255);
        };
    } // namespace scene
} // namespace ouzel
