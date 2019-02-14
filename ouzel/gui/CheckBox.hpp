// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GUI_CHECKBOX_HPP
#define OUZEL_GUI_CHECKBOX_HPP

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

            void setEnabled(bool newEnabled) override;

            virtual void setChecked(bool newChecked);
            virtual bool isChecked() const { return checked; }

            inline scene::Sprite* getNormalSprite() const { return normalSprite.get(); }
            inline scene::Sprite* getSelectedSprite() const { return selectedSprite.get(); }
            inline scene::Sprite* getPressedSprite() const { return pressedSprite.get(); }
            inline scene::Sprite* getDisabledSprite() const { return disabledSprite.get(); }

        protected:
            bool handleUI(const UIEvent& event);

            void updateSprite();

            std::unique_ptr<scene::Sprite> normalSprite;
            std::unique_ptr<scene::Sprite> selectedSprite;
            std::unique_ptr<scene::Sprite> pressedSprite;
            std::unique_ptr<scene::Sprite> disabledSprite;
            std::unique_ptr<scene::Sprite> tickSprite;

            EventHandler eventHandler;

            bool pointerOver = false;
            bool pressed = false;
            bool checked = false;
        };
    } // namespace gui
} // namespace ouzel

#endif // OUZEL_GUI_CHECKBOX_HPP
