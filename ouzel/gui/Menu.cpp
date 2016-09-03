// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Menu.h"
#include "core/Engine.h"
#include "events/EventDispatcher.h"

namespace ouzel
{
    namespace gui
    {
        Menu::Menu()
        {
            eventHandler.keyboardHandler = std::bind(&Menu::handleKeyboard, this, std::placeholders::_1, std::placeholders::_2);
            eventHandler.gamepadHandler = std::bind(&Menu::handleGamepad, this, std::placeholders::_1, std::placeholders::_2);
            eventHandler.uiHandler = std::bind(&Menu::handleUI, this, std::placeholders::_1, std::placeholders::_2);
            sharedEngine->getEventDispatcher()->addEventHandler(eventHandler);
        }

        Menu::~Menu()
        {
        }

        bool Menu::addWidget(const WidgetPtr& widget)
        {
            if (!addChild(widget))
            {
                return false;
            }

            widgets.push_back(widget);

            if (!selectedWidget)
            {
                selectWidget(widget);
            }

            return true;
        }

        bool Menu::removeWidget(const WidgetPtr& widget)
        {
            if (!removeChild(widget))
            {
                return false;
            }

            widgets.remove(widget);

            if (selectedWidget == widget)
            {
                selectWidget(nullptr);
            }

            return true;
        }

        void Menu::selectWidget(const WidgetPtr& widget)
        {
            selectedWidget = widget;

            for (const WidgetPtr& widget : widgets)
            {
                widget->setSelected(widget == selectedWidget);
            }
        }

        void Menu::selectNextWidget()
        {
            if (widgets.empty()) return;

            std::list<WidgetPtr>::iterator widgetIterator = widgets.end();

            if (selectedWidget)
            {
                widgetIterator = std::find(widgets.begin(), widgets.end(), selectedWidget);
            }

            if (widgetIterator != widgets.end()) widgetIterator++;
            if (widgetIterator == widgets.end()) widgetIterator = widgets.begin();

            selectWidget(*widgetIterator);
        }

        void Menu::selectPreviousWidget()
        {
            if (widgets.empty()) return;

            std::list<WidgetPtr>::iterator widgetIterator = widgets.end();

            if (selectedWidget)
            {
                widgetIterator = std::find(widgets.begin(), widgets.end(), selectedWidget);
            }

            if (widgetIterator == widgets.begin())
            {
                widgetIterator = --widgets.end();
            }
            else
            {
                widgetIterator--;
            }

            selectWidget(*widgetIterator);
        }

        bool Menu::handleKeyboard(Event::Type type, const KeyboardEvent& event)
        {
            if (!enabled) return true;

            if (type == Event::Type::KEY_DOWN && !widgets.empty())
            {
                if (event.key == input::KeyboardKey::UP)
                {
                    selectPreviousWidget();
                }
                else if (event.key == input::KeyboardKey::DOWN)
                {
                    selectNextWidget();
                }
                else if (event.key == input::KeyboardKey::RETURN)
                {
                    if (selectedWidget)
                    {
                        Event clickEvent;
                        clickEvent.type = Event::Type::UI_CLICK_NODE;

                        clickEvent.uiEvent.node = selectedWidget;
                        clickEvent.uiEvent.position = selectedWidget->getPosition();

                        sharedEngine->getEventDispatcher()->dispatchEvent(clickEvent);
                    }
                }
            }

            return true;
        }

        bool Menu::handleGamepad(Event::Type, const GamepadEvent&)
        {
            if (!enabled) return true;

            return true;
        }

        bool Menu::handleUI(Event::Type type, const UIEvent& event)
        {
            if (!enabled) return true;

            if (type == Event::Type::UI_ENTER_NODE)
            {
                if (std::find(widgets.begin(), widgets.end(), event.node) != widgets.end())
                {
                    selectWidget(std::static_pointer_cast<Widget>(event.node));
                }
            }

            return true;
        }
    } // namespace gui
} // namespace ouzel
