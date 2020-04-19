// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GUI_BUTTON_HPP
#define OUZEL_GUI_BUTTON_HPP

#include <functional>
#include "Widget.hpp"
#include "../scene/SpriteRenderer.hpp"
#include "../scene/TextRenderer.hpp"
#include "../events/Event.hpp"
#include "../events/EventHandler.hpp"
#include "../math/Color.hpp"

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
                   Color initLabelColor = Color::white(),
                   Color initLabelSelectedColor = Color::white(),
                   Color initLabelPressedColor = Color::white(),
                   Color initLabelDisabledColor = Color::white());

            void setEnabled(bool newEnabled) override;

            inline auto getNormalSprite() const noexcept { return normalSprite.get(); }
            inline auto getSelectedSprite() const noexcept { return selectedSprite.get(); }
            inline auto getPressedSprite() const noexcept { return pressedSprite.get(); }
            inline auto getDisabledSprite() const noexcept { return disabledSprite.get(); }
            inline auto getLabelDrawable() const noexcept { return labelDrawable.get(); }

        private:
            void setSelected(bool newSelected) override;

            bool handleUI(const UIEvent& event);

            void updateSprite();

            std::unique_ptr<scene::SpriteRenderer> normalSprite;
            std::unique_ptr<scene::SpriteRenderer> selectedSprite;
            std::unique_ptr<scene::SpriteRenderer> pressedSprite;
            std::unique_ptr<scene::SpriteRenderer> disabledSprite;
            std::unique_ptr<scene::TextRenderer> labelDrawable;

            EventHandler eventHandler;

            bool pointerOver = false;
            bool pressed = false;

            Color labelColor = Color::white();
            Color labelSelectedColor = Color::white();
            Color labelPressedColor = Color::white();
            Color labelDisabledColor = Color::white();
        };

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
            virtual bool isChecked() const noexcept { return checked; }

            inline auto getNormalSprite() const noexcept { return normalSprite.get(); }
            inline auto getSelectedSprite() const noexcept { return selectedSprite.get(); }
            inline auto getPressedSprite() const noexcept { return pressedSprite.get(); }
            inline auto getDisabledSprite() const noexcept { return disabledSprite.get(); }

        private:
            bool handleUI(const UIEvent& event);

            void updateSprite();

            std::unique_ptr<scene::SpriteRenderer> normalSprite;
            std::unique_ptr<scene::SpriteRenderer> selectedSprite;
            std::unique_ptr<scene::SpriteRenderer> pressedSprite;
            std::unique_ptr<scene::SpriteRenderer> disabledSprite;
            std::unique_ptr<scene::SpriteRenderer> tickSprite;

            EventHandler eventHandler;

            bool pointerOver = false;
            bool pressed = false;
            bool checked = false;
        };

        class ComboBox: public Widget
        {
        public:
            ComboBox();
        };

        class EditBox: public Widget
        {
        public:
            EditBox();

            inline auto& getValue() const noexcept { return value; }
            void setValue(const std::string& newValue);

        private:
            std::string value;
        };

        class Label: public Widget
        {
        public:
            Label(const std::string& initText,
                  const std::string& fontFile,
                  float fontSize = 1.0F,
                  Color color = Color::white(),
                  const Vector2F& textAnchor = Vector2F{0.5F, 0.5F});

            inline auto& getText() const noexcept { return text; }
            virtual void setText(const std::string& newText);

            inline auto& getLabelDrawable() const noexcept { return labelDrawable; }

        private:
            std::string text;
            std::shared_ptr<scene::TextRenderer> labelDrawable;
        };

        class Menu: public Widget
        {
        public:
            Menu();

            void setEnabled(bool newEnabled) override;

            bool removeChild(const Actor* actor) override;

            void addWidget(Widget* widget);

            void selectWidget(Widget* widget);
            void selectNextWidget();
            void selectPreviousWidget();

        protected:
            void enter() override;
            void leave() override;

            bool handleKeyboard(const KeyboardEvent& event);
            bool handleGamepad(const GamepadEvent& event);
            bool handleUI(const UIEvent& event);

            std::vector<Widget*> widgets;
            Widget* selectedWidget = nullptr;

            EventHandler eventHandler;
        };

        class RadioButton: public Widget
        {
        public:
            RadioButton();

            void setEnabled(bool newEnabled) override;

            void setChecked(bool newChecked);
            inline auto isChecked() const noexcept { return checked; }

        private:
            bool pointerOver = false;
            bool pressed = false;
            bool checked = false;
        };

        class RadioButtonGroup: public Widget
        {
        public:
            RadioButtonGroup();
        };

        class ScrollArea: public Widget
        {
        public:
            ScrollArea();
        };

        class ScrollBar: public Widget
        {
        public:
            ScrollBar();
        };

        class SlideBar: public Widget
        {
        public:
            SlideBar();
        };
    } // namespace gui
} // namespace ouzel

#endif // OUZEL_GUI_BUTTON_HPP
