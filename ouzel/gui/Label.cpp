// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"
#include "Label.h"
#include "core/Engine.h"
#include "graphics/Renderer.h"
#include "scene/Layer.h"
#include "scene/Camera.h"
#include "BMFont.h"
#include "core/Cache.h"

namespace ouzel
{
    namespace gui
    {
        Label::Label(const std::string& aText,
                     const std::string& fontFile,
                     Color color,
                     const Vector2& textAnchor):
            text(aText),
            labelDrawable(std::make_shared<scene::TextRenderer>(fontFile, true, text, color, textAnchor))
        {
            addComponent(labelDrawable.get());
            labelDrawable->setText(text);

            pickable = true;
        }

		Label::Label(const std::string & aText, const std::string & fontFile, int pt, Color color, const Vector2 & textAnchor) :
			text(aText),
			labelDrawable(std::make_shared<scene::TextRenderer>(fontFile, pt,true, text, color, textAnchor))
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
