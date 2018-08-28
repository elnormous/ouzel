// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"
#include "Label.hpp"
#include "core/Engine.hpp"
#include "graphics/Renderer.hpp"
#include "scene/Layer.hpp"
#include "scene/Camera.hpp"
#include "BMFont.hpp"
#include "assets/Cache.hpp"

namespace ouzel
{
    namespace gui
    {
        Label::Label(const std::string& initText,
                     const std::string& fontFile,
                     float fontSize,
                     Color color,
                     const Vector2& textAnchor):
            text(initText),
            labelDrawable(std::make_shared<scene::TextRenderer>(fontFile, fontSize, text, color, textAnchor))
        {
            addComponent(labelDrawable.get());
            labelDrawable->setText(text);

            pickable = true;
        }

        void Label::setText(const std::string& newText)
        {
            text = newText;
            labelDrawable->setText(text);
        }
    } // namespace gui
} // namespace ouzel
