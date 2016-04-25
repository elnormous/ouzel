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
#include "TextDrawable.h";

namespace ouzel
{
    namespace gui
    {
        std::shared_ptr<Label> Label::create(const std::string& font, const std::string& text, const Vector2& textAnchor)
        {
            std::shared_ptr<Label> result = std::make_shared<Label>();

            if (!result->init(font, text, textAnchor))
            {
                result.reset();
            }

            return result;
        }

        Label::Label()
        {
            
        }

        Label::~Label()
        {

        }

        bool Label::init(const std::string& font, const std::string& text, const Vector2& textAnchor)
        {
            _text = text;
            _textDrawable = scene::TextDrawable::create(font, text, textAnchor);

            if (!_textDrawable)
            {
                return false;
            }

            addDrawable(_textDrawable);

            _textDrawable->setText(_text);

            return true;
        }

        void Label::setText(const std::string& text)
        {
            _text = text;
            _textDrawable->setText(_text);
        }

        void Label::setColor(const graphics::Color& color)
        {
            _color = color;
            _textDrawable->setColor(_color);
        }
    } // namespace gui
} // namespace ouzel
