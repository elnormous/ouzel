// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "CompileConfig.h"
#include "Label.h"
#include "Engine.h"
#include "Renderer.h"
#include "Layer.h"
#include "Camera.h"
#include "BMFont.h"
#include "Cache.h"
#include "TextDrawable.h"

namespace ouzel
{
    namespace gui
    {
        LabelPtr Label::create(const std::string& font, const std::string& text, const Vector2& textAnchor)
        {
            LabelPtr result = std::make_shared<Label>();
            result->init(font, text, textAnchor);

            return result;
        }

        Label::Label()
        {
            
        }

        Label::~Label()
        {

        }

        bool Label::init(const std::string& fontFile, const std::string& newText, const Vector2& newTextAnchor)
        {
            text = newText;
            textDrawable = scene::TextDrawable::create(fontFile, text, newTextAnchor);

            if (!textDrawable)
            {
                return false;
            }

            addDrawable(textDrawable);

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
