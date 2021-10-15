// Ouzel by Elviss Strazdins

#ifndef OUZEL_GUI_BUTTON_HPP
#define OUZEL_GUI_BUTTON_HPP

#include <functional>
#include <memory>
#include "Widget.hpp"
#include "../scene/SpriteRenderer.hpp"
#include "../scene/TextRenderer.hpp"
#include "../events/Event.hpp"
#include "../events/EventHandler.hpp"
#include "../math/Color.hpp"

namespace ouzel::gui
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
               math::Color initLabelColor = math::whiteColor(),
               math::Color initLabelSelectedColor = math::whiteColor(),
               math::Color initLabelPressedColor = math::whiteColor(),
               math::Color initLabelDisabledColor = math::whiteColor());

        void setEnabled(bool newEnabled) override;

        auto getNormalSprite() const noexcept { return normalSprite.get(); }
        auto getSelectedSprite() const noexcept { return selectedSprite.get(); }
        auto getPressedSprite() const noexcept { return pressedSprite.get(); }
        auto getDisabledSprite() const noexcept { return disabledSprite.get(); }
        auto getLabelDrawable() const noexcept { return labelDrawable.get(); }

    private:
        void setSelected(bool newSelected) override;

        bool handleUi(const UIEvent& event);

        void updateSprite();

        std::unique_ptr<scene::SpriteRenderer> normalSprite;
        std::unique_ptr<scene::SpriteRenderer> selectedSprite;
        std::unique_ptr<scene::SpriteRenderer> pressedSprite;
        std::unique_ptr<scene::SpriteRenderer> disabledSprite;
        std::unique_ptr<scene::TextRenderer> labelDrawable;

        EventHandler eventHandler;

        bool pointerOver = false;
        bool pressed = false;

        math::Color labelColor = math::whiteColor();
        math::Color labelSelectedColor = math::whiteColor();
        math::Color labelPressedColor = math::whiteColor();
        math::Color labelDisabledColor = math::whiteColor();
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

        auto getNormalSprite() const noexcept { return normalSprite.get(); }
        auto getSelectedSprite() const noexcept { return selectedSprite.get(); }
        auto getPressedSprite() const noexcept { return pressedSprite.get(); }
        auto getDisabledSprite() const noexcept { return disabledSprite.get(); }

    private:
        bool handleUi(const UIEvent& event);

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

        auto& getValue() const noexcept { return value; }
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
              math::Color color = math::whiteColor(),
              const math::Vector<float, 2>& textAnchor = math::Vector<float, 2>{0.5F, 0.5F});

        auto& getText() const noexcept { return text; }
        virtual void setText(const std::string& newText);

        auto& getLabelDrawable() noexcept { return labelDrawable; }
        auto& getLabelDrawable() const noexcept { return labelDrawable; }

    private:
        std::string text;
        scene::TextRenderer labelDrawable;
    };

    class Menu: public Widget
    {
    public:
        Menu();

        void setEnabled(bool newEnabled) override;

        bool removeChild(const Actor& actor) override;

        void addWidget(Widget& widget);

        void selectWidget(Widget* widget);
        void selectNextWidget();
        void selectPreviousWidget();

    protected:
        void enter() override;
        void leave() override;

        bool handleKeyboard(const KeyboardEvent& event);
        bool handleGamepad(const GamepadEvent& event);
        bool handleUi(const UIEvent& event);

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
        auto isChecked() const noexcept { return checked; }

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
}

#endif // OUZEL_GUI_BUTTON_HPP
