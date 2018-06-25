// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <memory>
#include <string>
#include "gui/Widget.hpp"
#include "math/Color.hpp"
#include "scene/TextRenderer.hpp"

namespace ouzel
{
    namespace gui
    {
        class Label: public Widget
        {
        public:
            Label(const std::string& initText,
                  const std::string& fontFile,
                  float fontSize = 1.0F,
                  Color color = Color::WHITE,
                  const Vector2& textAnchor = Vector2(0.5F, 0.5F));

            inline const std::string& getText() const { return text; }
            virtual void setText(const std::string& newText);

            inline const std::shared_ptr<scene::TextRenderer>& getLabelDrawable() const { return labelDrawable; }

        protected:
            std::string text;
            std::shared_ptr<scene::TextRenderer> labelDrawable;
        };
    } // namespace gui
} // namespace ouzel
