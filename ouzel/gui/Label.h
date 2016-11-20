// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include <string>
#include "gui/Widget.h"
#include "utils/Types.h"
#include "graphics/Color.h"
#include "scene/TextDrawable.h"

namespace ouzel
{
    namespace gui
    {
        class Label: public Widget
        {
        public:
            Label(const std::string& fontFile, const std::string& pText, const Vector2& textAnchor = Vector2(0.5f, 0.5f));

            virtual void setText(const std::string& newText);
            virtual const std::string& getText() const { return text; }

        protected:
            std::string text;
            scene::TextDrawable textDrawable;
        };
    } // namespace gui
} // namespace ouzel
