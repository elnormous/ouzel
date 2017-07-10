// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Menu.h"
#include "core/Engine.h"
#include "events/EventDispatcher.h"
#include "input/Input.h"
#include "utils/Log.h"

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
            sharedEngine->getEventDispatcher()->addEventHandler(&eventHandler);
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
                {
                    selectWidget(widgets.front());
                }
            }
            else
            {
                selectedWidget = nullptr;

                for (Widget* childWidget : widgets)
                {
                    childWidget->setSelected(false);
                }
            }
        }

        void Menu::addChildWidget(Widget* widget)
        {
            addChild(widget);

            if (widget)
            {
                if (widget->menu)
                {
                    widget->menu->removeChild(widget);
                }

                widget->menu = this;
                widgets.push_back(widget);

                if (!selectedWidget)
                {
                    selectWidget(widget);
                }
            }
        }

        bool Menu::removeChildNode(Node* node)
        {
            auto i = std::find(widgets.begin(), widgets.end(), node);

            if (i != widgets.end())
            {
                widgets.erase(i);

                Widget* widget = static_cast<Widget*>(node);
                widget->menu = nullptr;
            }

            if (selectedWidget == node)
            {
                selectWidget(nullptr);
            }

            if (!Node::removeChildNode(node))
            {
                return false;
            }

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
                {
                    childWidget->setSelected(false);
                }
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
                {
                    widgetIterator = widgets.begin();
                }
                else
                {
                    widgetIterator++;
                }

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

            if (type == Event::Type::KEY_DOWN && !widgets.empty())
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
                    {
                        if (selectedWidget)
                        {
                            Event clickEvent;
                            clickEvent.type = Event::Type::CLICK_NODE;

                            clickEvent.uiEvent.node = selectedWidget;
                            clickEvent.uiEvent.position = selectedWidget->getPosition();

                            sharedEngine->getEventDispatcher()->postEvent(clickEvent);
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
                if (event.pressed)
                {
                    if (event.button == input::GamepadButton::FACE1)
                    {
                        if (selectedWidget)
                        {
                            Event clickEvent;
                            clickEvent.type = Event::Type::CLICK_NODE;

                            clickEvent.uiEvent.node = selectedWidget;
                            clickEvent.uiEvent.position = selectedWidget->getPosition();

                            sharedEngine->getEventDispatcher()->postEvent(clickEvent);
                        }
                    }
                    else if (event.button == input::GamepadButton::DPAD_LEFT ||
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
                        if (event.previousValue < 0.6f && event.value > 0.6f) selectPreviousWidget();
                    }
                    else if (event.button == input::GamepadButton::LEFT_THUMB_RIGHT ||
                             event.button == input::GamepadButton::LEFT_THUMB_DOWN)
                    {
                        if (event.previousValue < 0.6f && event.value > 0.6f) selectNextWidget();
                    }
                }
            }

            return true;
        }

        bool Menu::handleUI(Event::Type type, const UIEvent& event)
        {
            if (!enabled) return true;

            if (type == Event::Type::ENTER_NODE)
            {
                if (std::find(widgets.begin(), widgets.end(), event.node) != widgets.end())
                {
                    selectWidget(static_cast<Widget*>(event.node));
                }
            }

            return true;
        }
    } // namespace gui
} // namespace ouzel
