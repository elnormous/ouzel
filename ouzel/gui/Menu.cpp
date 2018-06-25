// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Menu.hpp"
#include "core/Engine.hpp"
#include "events/EventDispatcher.hpp"
#include "input/InputManager.hpp"

namespace ouzel
{
    namespace gui
    {
        Menu::Menu()
        {
            eventHandler.keyboardHandler = std::bind(&Menu::handleKeyboard, this, std::placeholders::_1, std::placeholders::_2);
            eventHandler.gamepadHandler = std::bind(&Menu::handleGamepad, this, std::placeholders::_1, std::placeholders::_2);
            eventHandler.uiHandler = std::bind(&Menu::handleUI, this, std::placeholders::_1, std::placeholders::_2);
        }

        void Menu::enter()
        {
            engine->getEventDispatcher()->addEventHandler(&eventHandler);
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

            std::vector<Widget*>::iterator firstWidgetIterator = selectedWidget ?
                std::find(widgets.begin(), widgets.end(), selectedWidget) :
                widgets.end();

            std::vector<Widget*>::iterator widgetIterator = firstWidgetIterator;

            do
            {
                if (widgetIterator == widgets.end())
                    widgetIterator = widgets.begin();
                else
                    widgetIterator++;

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

            std::vector<Widget*>::iterator firstWidgetIterator = selectedWidget ?
                std::find(widgets.begin(), widgets.end(), selectedWidget) :
                widgets.end();

            std::vector<Widget*>::iterator widgetIterator = firstWidgetIterator;

            do
            {
                if (widgetIterator == widgets.begin()) widgetIterator = widgets.end();
                if (widgetIterator != widgets.begin()) widgetIterator--;

                if (widgetIterator != widgets.end() && (*widgetIterator)->isEnabled())
                {
                    selectWidget(*widgetIterator);
                    break;
                }
            }
            while (widgetIterator != firstWidgetIterator);
        }

        bool Menu::handleKeyboard(Event::Type type, const KeyboardEvent& event)
        {
            if (!enabled) return true;

            if (type == Event::Type::KEY_PRESS && !widgets.empty())
            {
                switch (event.key)
                {
                    case input::KeyboardKey::LEFT:
                    case input::KeyboardKey::UP:
                        selectPreviousWidget();
                        break;
                    case input::KeyboardKey::RIGHT:
                    case input::KeyboardKey::DOWN:
                        selectNextWidget();
                        break;
                    case input::KeyboardKey::RETURN:
                    case input::KeyboardKey::SPACE:
                    case input::KeyboardKey::SELECT:
                    {
                        if (selectedWidget)
                        {
                            Event clickEvent;
                            clickEvent.type = Event::Type::ACTOR_CLICK;

                            clickEvent.uiEvent.actor = selectedWidget;
                            clickEvent.uiEvent.position = selectedWidget->getPosition();

                            engine->getEventDispatcher()->postEvent(clickEvent);
                        }
                        break;
                    }
                    default:
                        break;
                }
            }

            return true;
        }

        bool Menu::handleGamepad(Event::Type type, const GamepadEvent& event)
        {
            if (!enabled) return true;

            if (type == Event::Type::GAMEPAD_BUTTON_CHANGE)
            {
                if (event.button == input::GamepadButton::DPAD_LEFT ||
                    event.button == input::GamepadButton::DPAD_UP)
                {
                    if (!event.previousPressed && event.pressed) selectPreviousWidget();
                }
                else if (event.button == input::GamepadButton::DPAD_RIGHT ||
                         event.button == input::GamepadButton::DPAD_DOWN)
                {
                    if (!event.previousPressed && event.pressed) selectNextWidget();
                }
                else if (event.button == input::GamepadButton::LEFT_THUMB_LEFT ||
                         event.button == input::GamepadButton::LEFT_THUMB_UP)
                {
                    if (event.previousValue < 0.6F && event.value > 0.6F) selectPreviousWidget();
                }
                else if (event.button == input::GamepadButton::LEFT_THUMB_RIGHT ||
                         event.button == input::GamepadButton::LEFT_THUMB_DOWN)
                {
                    if (event.previousValue < 0.6F && event.value > 0.6F) selectNextWidget();
                }
#if !OUZEL_PLATFORM_IOS && !OUZEL_PLATFORM_TVOS // on iOS and tvOS menu items ar selected with a SELECT button
                else if (event.button == input::GamepadButton::FACE_BOTTOM)
                {
                    if (!event.previousPressed && event.pressed && selectedWidget)
                    {
                        Event clickEvent;
                        clickEvent.type = Event::Type::ACTOR_CLICK;

                        clickEvent.uiEvent.actor = selectedWidget;
                        clickEvent.uiEvent.position = selectedWidget->getPosition();

                        engine->getEventDispatcher()->postEvent(clickEvent);
                    }
                }
#endif
            }

            return true;
        }

        bool Menu::handleUI(Event::Type type, const UIEvent& event)
        {
            if (!enabled) return true;

            if (type == Event::Type::ACTOR_ENTER)
            {
                if (std::find(widgets.begin(), widgets.end(), event.actor) != widgets.end())
                    selectWidget(static_cast<Widget*>(event.actor));
            }

            return true;
        }
    } // namespace gui
} // namespace ouzel
