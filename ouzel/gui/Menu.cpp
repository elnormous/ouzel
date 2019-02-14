// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#if defined(__APPLE__)
#  include <TargetConditionals.h>
#endif
#include <algorithm>
#include "Menu.hpp"
#include "core/Engine.hpp"
#include "events/EventDispatcher.hpp"
#include "input/InputManager.hpp"

namespace ouzel
{
    namespace gui
    {
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
    } // namespace gui
} // namespace ouzel
