// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

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
            Label(const std::string& aText,
                  const std::string& fontFile,
                  Color color = Color::WHITE,
                  const Vector2& textAnchor = Vector2(0.5f, 0.5f));
			Label(const std::string& aText,
				const std::string& fontFile,
				int pt, 
				Color color = Color::WHITE,
				const Vector2& textAnchor = Vector2(0.5f, 0.5f));


            virtual void setText(const std::string& newText);
            virtual const std::string& getText() const { return text; }

            const std::shared_ptr<scene::TextRenderer>& getLabelDrawable() { return labelDrawable; }

        protected:
            std::string text;
            std::shared_ptr<scene::TextRenderer> labelDrawable;
        };
    } // namespace gui
} // namespace ouzel
