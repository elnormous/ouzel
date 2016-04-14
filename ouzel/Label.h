// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include "Widget.h"
#include "BMFont.h"

namespace ouzel
{
    namespace gui
    {
        class Label: public Widget
        {
        public:
            static std::shared_ptr<Label> create(const std::string& font, const std::string& text, const Vector2& textAnchor = Vector2(0.5f, 0.5f));

            Label();
            virtual ~Label();

            virtual bool init(const std::string& font, const std::string& text, const Vector2& textAnchor = Vector2(0.5f, 0.5f));

            virtual void draw() override;

            virtual void setText(const std::string& text);
            virtual const std::string& getText() const { return _text; }

            virtual const video::Color& getColor() const { return _color; }
            virtual void setColor(const video::Color& color);

        protected:
            void updateMesh();

            video::TexturePtr _texture;
            video::MeshBufferPtr _meshBuffer;
            video::ShaderPtr _shader;
            
            BMFont _font;
            Vector2 _textAnchor;
            std::string _text;

            video::Color _color = video::Color(255, 255, 255, 255);
        };
    } // namespace gui
} // namespace ouzel
