// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include "gui/Widget.h"
#include "utils/Types.h"
#include "graphics/Color.h"

namespace ouzel
{
    namespace gui
    {
        class Label: public Widget
        {
        public:
            Label();
            Label(const std::string& font, const std::string& pText, const Vector2& textAnchor = Vector2(0.5f, 0.5f));
            virtual ~Label();

            virtual bool init(const std::string& fontFile, const std::string& newText, const Vector2& newTextAnchor = Vector2(0.5f, 0.5f));

            virtual void setText(const std::string& newText);
            virtual const std::string& getText() const { return text; }

            virtual const graphics::Color& getColor() const { return color; }
            virtual void setColor(const graphics::Color& newColor);

        protected:
            std::string text;
            graphics::Color color = graphics::Color(255, 255, 255, 255);

            scene::TextDrawablePtr textDrawable;
        };
    } // namespace gui
} // namespace ouzel
