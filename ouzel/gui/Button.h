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
            Button();
            Button(const std::string& normalImage, const std::string& selectedImage, const std::string& pressedImage, const std::string& disabledImage,
                   const std::string& label = "", const graphics::Color& labelColor = graphics::Color(255, 255, 255, 255), const std::string& font = "");
            virtual ~Button();

            virtual bool init(const std::string& normal, const std::string& selected, const std::string& pressed, const std::string& disabled,
                              const std::string& label = "", const graphics::Color& labelColor = graphics::Color(255, 255, 255, 255), const std::string& font = "");

            virtual void setEnabled(bool enabled) override;

        protected:
            bool handleUI(Event::Type type, const UIEvent& event);

            void updateSprite();

            scene::SpritePtr normalSprite;
            scene::SpritePtr selectedSprite;
            scene::SpritePtr pressedSprite;
            scene::SpritePtr disabledSprite;
            scene::TextDrawablePtr labelDrawable;

            EventHandler eventHandler;

            bool selected = false;
            bool pointerOver = false;
            bool pressed = false;
        };
    } // namespace gui
} // namespace ouzel
