// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include "Drawable.h"
#include "Types.h"
#include "Color.h"
#include "BMFont.h"

namespace ouzel
{
    namespace scene
    {
        class TextDrawable: public Drawable
        {
        public:
            static std::shared_ptr<TextDrawable> create(const std::string& font, const std::string& text, const Vector2& textAnchor = Vector2(0.5f, 0.5f));

            TextDrawable();
            virtual ~TextDrawable();

            bool init(const std::string& font, const std::string& text, const Vector2& textAnchor = Vector2(0.5f, 0.5f));

            virtual void draw(const Matrix4& projection, const Matrix4& transform) override;

            virtual void setText(const std::string& text);
            virtual const std::string& getText() const { return _text; }

            virtual const graphics::Color& getColor() const { return _color; }
            virtual void setColor(const graphics::Color& color);

        protected:
            void updateMesh();

            graphics::TexturePtr _texture;
            graphics::MeshBufferPtr _meshBuffer;
            graphics::ShaderPtr _shader;

            BMFont _font;
            Vector2 _textAnchor;
            std::string _text;

            graphics::Color _color = graphics::Color(255, 255, 255, 255);
        };
    } // namespace scene
} // namespace ouzel
