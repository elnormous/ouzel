// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include "Widget.h"
#include "Types.h"
#include "Color.h"

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

            virtual void setText(const std::string& text);
            virtual const std::string& getText() const { return _text; }

            virtual const graphics::Color& getColor() const { return _color; }
            virtual void setColor(const graphics::Color& color);

        protected:
            std::string _text;
            graphics::Color _color = graphics::Color(255, 255, 255, 255);

            scene::TextDrawablePtr _textDrawable;
        };
    } // namespace gui
} // namespace ouzel
