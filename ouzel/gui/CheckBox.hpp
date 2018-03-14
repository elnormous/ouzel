// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "gui/Widget.hpp"
#include "scene/Sprite.hpp"
#include "events/Event.hpp"
#include "events/EventHandler.hpp"

namespace ouzel
{
    namespace gui
    {
        class CheckBox: public Widget
        {
        public:
            CheckBox(const std::string& normalImage,
                     const std::string& selectedImage,
                     const std::string& pressedImage,
                     const std::string& disabledImage,
                     const std::string& tickImage);

            virtual void setEnabled(bool newEnabled) override;

            virtual void setChecked(bool newChecked);
            virtual bool isChecked() const { return checked; }

            inline scene::Sprite* getNormalSprite() const { return normalSprite.get(); }
            inline scene::Sprite* getSelectedSprite() const { return selectedSprite.get(); }
            inline scene::Sprite* getPressedSprite() const { return pressedSprite.get(); }
            inline scene::Sprite* getDisabledSprite() const { return disabledSprite.get(); }

        protected:
            bool handleUI(Event::Type type,const UIEvent& event);

            void updateSprite();

            UniquePtr<scene::Sprite> normalSprite;
            UniquePtr<scene::Sprite> selectedSprite;
            UniquePtr<scene::Sprite> pressedSprite;
            UniquePtr<scene::Sprite> disabledSprite;
            UniquePtr<scene::Sprite> tickSprite;

            EventHandler eventHandler;

            bool pointerOver = false;
            bool pressed = false;
            bool checked = false;
        };
    } // namespace gui
} // namespace ouzel
