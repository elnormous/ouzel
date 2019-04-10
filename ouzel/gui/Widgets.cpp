// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#if defined(__APPLE__)
#  include <TargetConditionals.h>
#endif
#include <algorithm>
#include <functional>
#include "Widgets.hpp"
#include "BMFont.hpp"
#include "core/Engine.hpp"
#include "events/EventHandler.hpp"
#include "events/EventDispatcher.hpp"
#include "math/Color.hpp"
#include "scene/Layer.hpp"
#include "scene/Camera.hpp"

namespace ouzel
{
    namespace gui
    {
        Button::Button():
            eventHandler(EventHandler::PRIORITY_MAX + 1)
        {
            eventHandler.uiHandler = std::bind(&Button::handleUI, this, std::placeholders::_1);
            engine->getEventDispatcher().addEventHandler(&eventHandler);

            pickable = true;
        }

        Button::Button(const std::string& normalImage,
                       const std::string& selectedImage,
                       const std::string& pressedImage,
                       const std::string& disabledImage,
                       const std::string& label,
                       const std::string& font,
                       float fontSize,
                       Color initLabelColor,
                       Color initLabelSelectedColor,
                       Color initLabelPressedColor,
                       Color initLabelDisabledColor):
            eventHandler(EventHandler::PRIORITY_MAX + 1),
            labelColor(initLabelColor),
            labelSelectedColor(initLabelSelectedColor),
            labelPressedColor(initLabelPressedColor),
            labelDisabledColor(initLabelDisabledColor)
        {
            eventHandler.uiHandler = std::bind(&Button::handleUI, this, std::placeholders::_1);
            engine->getEventDispatcher().addEventHandler(&eventHandler);

            if (!normalImage.empty())
            {
                normalSprite.reset(new scene::Sprite());
                normalSprite->init(normalImage);
                addComponent(normalSprite.get());
            }

            if (!selectedImage.empty())
            {
                selectedSprite.reset(new scene::Sprite());
                selectedSprite->init(selectedImage);
                addComponent(selectedSprite.get());
            }

            if (!pressedImage.empty())
            {
                pressedSprite.reset(new scene::Sprite());
                pressedSprite->init(pressedImage);
                addComponent(pressedSprite.get());
            }

            if (!disabledImage.empty())
            {
                disabledSprite.reset(new scene::Sprite());
                disabledSprite->init(disabledImage);
                addComponent(disabledSprite.get());
            }

            if (!label.empty())
            {
                labelDrawable.reset(new scene::TextRenderer(font, fontSize, label));
                labelDrawable->setColor(labelColor);
                addComponent(labelDrawable.get());
            }

            pickable = true;

            updateSprite();
        }

        void Button::setEnabled(bool newEnabled)
        {
            Widget::setEnabled(newEnabled);

            selected = false;
            pointerOver = false;
            pressed = false;

            updateSprite();
        }

        void Button::setSelected(bool newSelected)
        {
            Widget::setSelected(newSelected);

            updateSprite();
        }

        bool Button::handleUI(const UIEvent& event)
        {
            if (!enabled) return false;

            if (event.actor == this)
            {
                if (event.type == Event::Type::ACTOR_ENTER)
                {
                    pointerOver = true;
                    updateSprite();
                }
                else if (event.type == Event::Type::ACTOR_LEAVE)
                {
                    pointerOver = false;
                    updateSprite();
                }
                else if (event.type == Event::Type::ACTOR_PRESS)
                {
                    pressed = true;
                    updateSprite();
                }
                else if (event.type == Event::Type::ACTOR_RELEASE ||
                         event.type == Event::Type::ACTOR_CLICK)
                {
                    if (pressed)
                    {
                        pressed = false;
                        updateSprite();
                    }
                }
            }

            return false;
        }

        void Button::updateSprite()
        {
            if (normalSprite) normalSprite->setHidden(true);
            if (selectedSprite) selectedSprite->setHidden(true);
            if (pressedSprite) pressedSprite->setHidden(true);
            if (disabledSprite) disabledSprite->setHidden(true);

            if (enabled)
            {
                if (pressed && pointerOver && pressedSprite)
                    pressedSprite->setHidden(false);
                else if (selected && selectedSprite)
                    selectedSprite->setHidden(false);
                else if (normalSprite)
                    normalSprite->setHidden(false);

                if (labelDrawable)
                {
                    if (pressed && pointerOver)
                        labelDrawable->setColor(labelPressedColor);
                    else if (selected)
                        labelDrawable->setColor(labelSelectedColor);
                    else
                        labelDrawable->setColor(labelColor);
                }
            }
            else // disabled
            {
                if (disabledSprite)
                    disabledSprite->setHidden(false);
                else if (normalSprite)
                    normalSprite->setHidden(false);

                if (labelDrawable)
                    labelDrawable->setColor(labelDisabledColor);
            }
        }

        CheckBox::CheckBox(const std::string& normalImage,
                           const std::string& selectedImage,
                           const std::string& pressedImage,
                           const std::string& disabledImage,
                           const std::string& tickImage):
            eventHandler(EventHandler::PRIORITY_MAX + 1)
        {
            eventHandler.uiHandler = std::bind(&CheckBox::handleUI, this, std::placeholders::_1);
            engine->getEventDispatcher().addEventHandler(&eventHandler);

            if (!normalImage.empty())
            {
                normalSprite.reset(new scene::Sprite());
                normalSprite->init(normalImage);
                addComponent(normalSprite.get());
            }

            if (!selectedImage.empty())
            {
                selectedSprite.reset(new scene::Sprite());
                selectedSprite->init(selectedImage);
                addComponent(selectedSprite.get());
            }

            if (!pressedImage.empty())
            {
                pressedSprite.reset(new scene::Sprite());
                pressedSprite->init(pressedImage);
                addComponent(pressedSprite.get());
            }

            if (!disabledImage.empty())
            {
                disabledSprite.reset(new scene::Sprite());
                disabledSprite->init(disabledImage);
                addComponent(disabledSprite.get());
            }

            if (!tickImage.empty())
            {
                tickSprite.reset(new scene::Sprite());
                tickSprite->init(tickImage);
                addComponent(tickSprite.get());
            }

            pickable = true;

            updateSprite();
        }

        void CheckBox::setEnabled(bool newEnabled)
        {
            Widget::setEnabled(newEnabled);

            selected = false;
            pointerOver = false;
            pressed = false;

            updateSprite();
        }

        void CheckBox::setChecked(bool newChecked)
        {
            checked = newChecked;

            updateSprite();
        }

        bool CheckBox::handleUI(const UIEvent& event)
        {
            if (!enabled) return false;

            if (event.actor == this)
            {
                if (event.type == Event::Type::ACTOR_ENTER)
                {
                    pointerOver = true;
                    updateSprite();
                }
                else if (event.type == Event::Type::ACTOR_LEAVE)
                {
                    pointerOver = false;
                    updateSprite();
                }
                else if (event.type == Event::Type::ACTOR_PRESS)
                {
                    pressed = true;
                    updateSprite();
                }
                else if (event.type == Event::Type::ACTOR_RELEASE)
                {
                    if (pressed)
                    {
                        pressed = false;
                        updateSprite();
                    }
                }
                else if (event.type == Event::Type::ACTOR_CLICK)
                {
                    pressed = false;
                    checked = !checked;
                    updateSprite();

                    std::unique_ptr<UIEvent> changeEvent(new UIEvent());
                    changeEvent->type = Event::Type::WIDGET_CHANGE;
                    changeEvent->actor = event.actor;
                    engine->getEventDispatcher().dispatchEvent(std::move(changeEvent));
                }
            }

            return false;
        }

        void CheckBox::updateSprite()
        {
            if (enabled)
            {
                if (pressed && pointerOver && pressedSprite)
                    pressedSprite->setHidden(false);
                else if (selected && selectedSprite)
                    selectedSprite->setHidden(false);
                else if (normalSprite)
                    normalSprite->setHidden(false);
            }
            else
            {
                if (disabledSprite)
                    disabledSprite->setHidden(false);
                else if (normalSprite)
                    normalSprite->setHidden(false);
            }

            if (tickSprite)
                tickSprite->setHidden(!checked);
        }

        ComboBox::ComboBox()
        {
            pickable = true;
        }

        EditBox::EditBox()
        {
            pickable = true;
        }

        void EditBox::setValue(const std::string& newValue)
        {
            value = newValue;
        }

        Label::Label(const std::string& initText,
                     const std::string& fontFile,
                     float fontSize,
                     Color color,
                     const Vector2F& textAnchor):
            text(initText),
            labelDrawable(std::make_shared<scene::TextRenderer>(fontFile, fontSize, text, color, textAnchor))
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

        Menu::Menu():
            eventHandler(EventHandler::PRIORITY_MAX + 1)
        {
            eventHandler.keyboardHandler = std::bind(&Menu::handleKeyboard, this, std::placeholders::_1);
            eventHandler.gamepadHandler = std::bind(&Menu::handleGamepad, this, std::placeholders::_1);
            eventHandler.uiHandler = std::bind(&Menu::handleUI, this, std::placeholders::_1);
        }

        void Menu::enter()
        {
            engine->getEventDispatcher().addEventHandler(&eventHandler);
        }

        void Menu::leave()
        {
            eventHandler.remove();
        }

        void Menu::setEnabled(bool newEnabled)
        {
            Widget::setEnabled(newEnabled);

            if (enabled)
            {
                if (!selectedWidget && !widgets.empty())
                    selectWidget(widgets.front());
            }
            else
            {
                selectedWidget = nullptr;

                for (Widget* childWidget : widgets)
                    childWidget->setSelected(false);
            }
        }

        void Menu::addWidget(Widget* widget)
        {
            addChild(widget);

            if (widget)
            {
                if (widget->menu)
                    widget->menu->removeChild(widget);

                widget->menu = this;
                widgets.push_back(widget);

                if (!selectedWidget)
                    selectWidget(widget);
            }
        }

        bool Menu::removeChild(Actor* actor)
        {
            auto i = std::find(widgets.begin(), widgets.end(), actor);

            if (i != widgets.end())
            {
                widgets.erase(i);

                Widget* widget = static_cast<Widget*>(actor);
                widget->menu = nullptr;
            }

            if (selectedWidget == actor)
                selectWidget(nullptr);

            if (!Actor::removeChild(actor))
                return false;

            return true;
        }

        void Menu::selectWidget(Widget* widget)
        {
            if (!enabled) return;

            selectedWidget = nullptr;

            for (Widget* childWidget : widgets)
            {
                if (childWidget == widget)
                {
                    selectedWidget = widget;
                    childWidget->setSelected(true);
                }
                else
                    childWidget->setSelected(false);
            }
        }

        void Menu::selectNextWidget()
        {
            if (!enabled) return;

            auto firstWidgetIterator = selectedWidget ?
            std::find(widgets.begin(), widgets.end(), selectedWidget) :
            widgets.end();

            auto widgetIterator = firstWidgetIterator;

            do
            {
                if (widgetIterator == widgets.end())
                    widgetIterator = widgets.begin();
                else
                    ++widgetIterator;

                if (widgetIterator != widgets.end() && (*widgetIterator)->isEnabled())
                {
                    selectWidget(*widgetIterator);
                    break;
                }
            }
            while (widgetIterator != firstWidgetIterator);
        }

        void Menu::selectPreviousWidget()
        {
            if (!enabled) return;

            auto firstWidgetIterator = selectedWidget ?
            std::find(widgets.begin(), widgets.end(), selectedWidget) :
            widgets.end();

            auto widgetIterator = firstWidgetIterator;

            do
            {
                if (widgetIterator == widgets.begin()) widgetIterator = widgets.end();
                if (widgetIterator != widgets.begin()) --widgetIterator;

                if (widgetIterator != widgets.end() && (*widgetIterator)->isEnabled())
                {
                    selectWidget(*widgetIterator);
                    break;
                }
            }
            while (widgetIterator != firstWidgetIterator);
        }

        bool Menu::handleKeyboard(const KeyboardEvent& event)
        {
            if (!enabled) return false;

            if (event.type == Event::Type::KEY_PRESS && !widgets.empty())
            {
                switch (event.key)
                {
                    case input::Keyboard::Key::LEFT:
                    case input::Keyboard::Key::UP:
                        selectPreviousWidget();
                        break;
                    case input::Keyboard::Key::RIGHT:
                    case input::Keyboard::Key::DOWN:
                        selectNextWidget();
                        break;
                    case input::Keyboard::Key::ENTER:
                    case input::Keyboard::Key::SPACE:
                    case input::Keyboard::Key::SELECT:
                    {
                        if (selectedWidget)
                        {
                            std::unique_ptr<UIEvent> clickEvent(new UIEvent());
                            clickEvent->type = Event::Type::ACTOR_CLICK;
                            clickEvent->actor = selectedWidget;
                            clickEvent->position = Vector2F(selectedWidget->getPosition());
                            engine->getEventDispatcher().dispatchEvent(std::move(clickEvent));
                        }
                        break;
                    }
                    default:
                        break;
                }
            }

            return false;
        }

        bool Menu::handleGamepad(const GamepadEvent& event)
        {
            if (!enabled) return false;

            if (event.type == Event::Type::GAMEPAD_BUTTON_CHANGE)
            {
                if (event.button == input::Gamepad::Button::DPAD_LEFT ||
                    event.button == input::Gamepad::Button::DPAD_UP)
                {
                    if (!event.previousPressed && event.pressed) selectPreviousWidget();
                }
                else if (event.button == input::Gamepad::Button::DPAD_RIGHT ||
                         event.button == input::Gamepad::Button::DPAD_DOWN)
                {
                    if (!event.previousPressed && event.pressed) selectNextWidget();
                }
                else if (event.button == input::Gamepad::Button::LEFT_THUMB_LEFT ||
                         event.button == input::Gamepad::Button::LEFT_THUMB_UP)
                {
                    if (event.previousValue < 0.6F && event.value > 0.6F) selectPreviousWidget();
                }
                else if (event.button == input::Gamepad::Button::LEFT_THUMB_RIGHT ||
                         event.button == input::Gamepad::Button::LEFT_THUMB_DOWN)
                {
                    if (event.previousValue < 0.6F && event.value > 0.6F) selectNextWidget();
                }
#if !defined(__APPLE__) || (!TARGET_OS_IOS && !TARGET_OS_TV) // on iOS and tvOS menu items ar selected with a SELECT button
                else if (event.button == input::Gamepad::Button::FACE_BOTTOM)
                {
                    if (!event.previousPressed && event.pressed && selectedWidget)
                    {
                        std::unique_ptr<UIEvent> clickEvent(new UIEvent());
                        clickEvent->type = Event::Type::ACTOR_CLICK;
                        clickEvent->actor = selectedWidget;
                        clickEvent->position = Vector2F(selectedWidget->getPosition());
                        engine->getEventDispatcher().dispatchEvent(std::move(clickEvent));
                    }
                }
#endif
            }

            return false;
        }

        bool Menu::handleUI(const UIEvent& event)
        {
            if (!enabled) return false;

            if (event.type == Event::Type::ACTOR_ENTER)
                if (std::find(widgets.begin(), widgets.end(), event.actor) != widgets.end())
                    selectWidget(static_cast<Widget*>(event.actor));

            return false;
        }

        RadioButton::RadioButton()
        {
            pickable = true;
        }

        void RadioButton::setEnabled(bool newEnabled)
        {
            Widget::setEnabled(newEnabled);

            selected = false;
            pointerOver = false;
            pressed = false;
        }

        void RadioButton::setChecked(bool newChecked)
        {
            checked = newChecked;
        }

        RadioButtonGroup::RadioButtonGroup()
        {
        }

        ScrollArea::ScrollArea()
        {
        }

        ScrollBar::ScrollBar()
        {
            pickable = true;
        }

        SlideBar::SlideBar()
        {
            pickable = true;
        }
    } // namespace gui
} // namespace ouzel
