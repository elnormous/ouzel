// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GUI_LABEL_HPP
#define OUZEL_GUI_LABEL_HPP

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
                  const Vector2F& textAnchor = Vector2F(0.5F, 0.5F));

            inline const std::string& getText() const { return text; }
            virtual void setText(const std::string& newText);

            inline const std::unique_ptr<scene::TextRenderer>& getLabelDrawable() const { return labelDrawable; }

        protected:
            std::string text;
            std::unique_ptr<scene::TextRenderer> labelDrawable;
        };
    } // namespace gui
} // namespace ouzel

#endif // OUZEL_GUI_LABEL_HPP
