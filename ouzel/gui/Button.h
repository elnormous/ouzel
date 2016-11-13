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

            std::unique_ptr<scene::Sprite> normalSprite;
            std::unique_ptr<scene::Sprite> selectedSprite;
            std::unique_ptr<scene::Sprite> pressedSprite;
            std::unique_ptr<scene::Sprite> disabledSprite;
            std::unique_ptr<scene::TextDrawable> labelDrawable;

            EventHandler eventHandler;

            bool pointerOver = false;
            bool pressed = false;
        };
    } // namespace gui
} // namespace ouzel
