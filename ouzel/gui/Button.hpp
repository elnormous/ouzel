// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_BUTTON_HPP
#define OUZEL_BUTTON_HPP

#include <functional>
#include "gui/Widget.hpp"
#include "scene/Sprite.hpp"
#include "scene/TextRenderer.hpp"
#include "events/Event.hpp"
#include "events/EventHandler.hpp"
#include "math/Color.hpp"

namespace ouzel
{
    namespace gui
    {
        class Button: public Widget
        {
        public:
            Button();
            Button(const std::string& normalImage,
                   const std::string& selectedImage,
                   const std::string& pressedImage,
                   const std::string& disabledImage,
                   const std::string& label = "",
                   const std::string& font = "",
                   float fontSize = 1.0F,
                   Color initLabelColor = Color::WHITE,
                   Color initLabelSelectedColor = Color::WHITE,
                   Color initLabelPressedColor = Color::WHITE,
                   Color initLabelDisabledColor = Color::WHITE);

            void setEnabled(bool newEnabled) override;

            inline scene::Sprite* getNormalSprite() const { return normalSprite.get(); }
            inline scene::Sprite* getSelectedSprite() const { return selectedSprite.get(); }
            inline scene::Sprite* getPressedSprite() const { return pressedSprite.get(); }
            inline scene::Sprite* getDisabledSprite() const { return disabledSprite.get(); }
            inline scene::TextRenderer* getLabelDrawable() const { return labelDrawable.get(); }

        protected:
            void setSelected(bool newSelected) override;

            bool handleUI(const UIEvent& event);

            void updateSprite();

            std::unique_ptr<scene::Sprite> normalSprite;
            std::unique_ptr<scene::Sprite> selectedSprite;
            std::unique_ptr<scene::Sprite> pressedSprite;
            std::unique_ptr<scene::Sprite> disabledSprite;
            std::unique_ptr<scene::TextRenderer> labelDrawable;

            EventHandler eventHandler;

            bool pointerOver = false;
            bool pressed = false;

            Color labelColor = Color::WHITE;
            Color labelSelectedColor = Color::WHITE;
            Color labelPressedColor = Color::WHITE;
            Color labelDisabledColor = Color::WHITE;
        };
    } // namespace gui
} // namespace ouzel

#endif // OUZEL_BUTTON_HPP
