// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <functional>
#include "utils/Types.h"
#include "gui/Widget.h"
#include "events/Event.h"
#include "events/EventHandler.h"
#include "graphics/Color.h"

namespace ouzel
{
    class EventHandler;

    namespace gui
    {
        class Button: public Widget
        {
        public:
            Button(const std::string& normalImage, const std::string& selectedImage, const std::string& pressedImage, const std::string& disabledImage,
                   const std::string& label = "", const graphics::Color& labelColor = graphics::Color::WHITE, const std::string& font = "");

            virtual void setEnabled(bool newEnabled) override;
            virtual void setSelected(bool newSelected) override;

        protected:
            bool handleUI(Event::Type type, const UIEvent& event);

            void updateSprite();

            scene::SpritePtr normalSprite;
            scene::SpritePtr selectedSprite;
            scene::SpritePtr pressedSprite;
            scene::SpritePtr disabledSprite;
            scene::TextDrawablePtr labelDrawable;

            EventHandler eventHandler;

            bool pointerOver = false;
            bool pressed = false;
        };
    } // namespace gui
} // namespace ouzel
