// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include <string>
#include "gui/Widget.h"
#include "math/Color.h"
#include "scene/TextDrawable.h"

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

            virtual void setText(const std::string& newText);
            virtual const std::string& getText() const { return text; }

            scene::TextDrawable* getLabelDrawable() { return &labelDrawable; }

        protected:
            std::string text;
            scene::TextDrawable labelDrawable;
        };
    } // namespace gui
} // namespace ouzel
