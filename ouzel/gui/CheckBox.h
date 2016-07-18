// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "gui/Widget.h"
#include "events/Event.h"
#include "events/EventHandler.h"

namespace ouzel
{
    namespace gui
    {
        class CheckBox: public Widget
        {
        public:
            CheckBox();
            CheckBox(const std::string& normalImage, const std::string& selectedImage, const std::string& pressedImage, const std::string& disabledImage, const std::string& tickImage);
            virtual ~CheckBox();

            virtual bool init(const std::string& normalImage, const std::string& selectedImage, const std::string& pressedImage, const std::string& disabledImage, const std::string& tickImage);

            virtual void setEnabled(bool enabled) override;

            virtual void setChecked(bool newChecked);
            virtual bool isChecked() const { return checked; }

        protected:
            bool handleGamepad(Event::Type type, const GamepadEvent& event, const VoidPtr& sender);
            bool handleUI(Event::Type type,const UIEvent& event, const VoidPtr& sender);

            void updateSprite();

            scene::SpritePtr normalSprite;
            scene::SpritePtr selectedSprite;
            scene::SpritePtr pressedSprite;
            scene::SpritePtr disabledSprite;
            scene::SpritePtr tickSprite;

            EventHandler eventHandler;

            bool selected = false;
            bool pointerOver = false;
            bool pressed = false;
            bool checked = false;
        };
    } // namespace gui
} // namespace ouzel
