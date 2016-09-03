// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"
#include "Label.h"
#include "core/Engine.h"
#include "graphics/Renderer.h"
#include "scene/Layer.h"
#include "scene/Camera.h"
#include "BMFont.h"
#include "core/Cache.h"
#include "scene/TextDrawable.h"

namespace ouzel
{
    namespace gui
    {
        Label::Label()
        {
        }

        Label::Label(const std::string& font, const std::string& pText, const Vector2& textAnchor)
        {
            init(font, pText, textAnchor);
        }

        Label::~Label()
        {
        }

        bool Label::init(const std::string& fontFile, const std::string& newText, const Vector2& newTextAnchor)
        {
            text = newText;
            textDrawable = std::make_shared<scene::TextDrawable>(fontFile, text, newTextAnchor);

            if (!textDrawable)
            {
                return false;
            }

            addComponent(textDrawable);

            textDrawable->setText(text);

            return true;
        }

        void Label::setText(const std::string& newText)
        {
            text = newText;
            textDrawable->setText(text);
        }

        void Label::setColor(const graphics::Color& newColor)
        {
            color = newColor;
            textDrawable->setColor(color);
        }
    } // namespace gui
} // namespace ouzel
