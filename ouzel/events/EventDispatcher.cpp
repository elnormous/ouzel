// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "EventDispatcher.h"

namespace ouzel
{
    EventDispatcher::EventDispatcher()
    {

    }

    EventDispatcher::~EventDispatcher()
    {

    }

    void EventDispatcher::update()
    {
        auto eventHandlersCopy = eventHandlers;
        Event event;

        while (true)
        {
            // scope for mutex lock
            {
                std::lock_guard<std::mutex> mutexLock(mutex);
                if (eventQueue.empty())
                {
                    break;
                }

                event = eventQueue.front();
                eventQueue.pop();
            }

            bool propagate = true;

            for (const EventHandlerPtr& eventHandler : eventHandlersCopy)
            {
                if (eventHandler)
                {
                    switch (event.type)
                    {
                        case Event::Type::KEY_DOWN:
                        case Event::Type::KEY_UP:
                        case Event::Type::KEY_REPEAT:
                            if (eventHandler->keyboardHandler)
                            {
                                propagate = eventHandler->keyboardHandler(event.type, event.keyboardEvent, event.sender);
                            }
                            break;

                        case Event::Type::MOUSE_DOWN:
                        case Event::Type::MOUSE_UP:
                        case Event::Type::MOUSE_SCROLL:
                        case Event::Type::MOUSE_MOVE:
                            if (eventHandler->mouseHandler)
                            {
                                propagate = eventHandler->mouseHandler(event.type, event.mouseEvent, event.sender);
                            }
                            break;
                        case Event::Type::TOUCH_BEGIN:
                        case Event::Type::TOUCH_MOVE:
                        case Event::Type::TOUCH_END:
                        case Event::Type::TOUCH_CANCEL:
                            if (eventHandler->touchHandler)
                            {
                                propagate = eventHandler->touchHandler(event.type, event.touchEvent, event.sender);
                            }
                            break;
                        case Event::Type::GAMEPAD_CONNECT:
                        case Event::Type::GAMEPAD_DISCONNECT:
                        case Event::Type::GAMEPAD_BUTTON_CHANGE:
                            if (eventHandler->gamepadHandler)
                            {
                                propagate = eventHandler->gamepadHandler(event.type, event.gamepadEvent, event.sender);
                            }
                            break;
                        case Event::Type::WINDOW_SIZE_CHANGE:
                        case Event::Type::WINDOW_TITLE_CHANGE:
                        case Event::Type::WINDOW_FULLSCREEN_CHANGE:
                            if (eventHandler->windowHandler)
                            {
                                propagate = eventHandler->windowHandler(event.type, event.windowEvent, event.sender);
                            }
                            break;
                        case Event::Type::LOW_MEMORY:
                        case Event::Type::OPEN_FILE:
                            if (eventHandler->systemHandler)
                            {
                                propagate = eventHandler->systemHandler(event.type, event.systemEvent, event.sender);
                            }
                            break;
                        case Event::Type::UI_ENTER_NODE:
                        case Event::Type::UI_LEAVE_NODE:
                        case Event::Type::UI_PRESS_NODE:
                        case Event::Type::UI_RELEASE_NODE:
                        case Event::Type::UI_CLICK_NODE:
                        case Event::Type::UI_DRAG_NODE:
                        case Event::Type::UI_WIDGET_CHANGE:
                            if (eventHandler->uiHandler)
                            {
                                propagate = eventHandler->uiHandler(event.type, event.uiEvent, event.sender);
                            }
                            break;
                    }
                }

                if (!propagate)
                {
                    break;
                }
            }
        }
    }

    void EventDispatcher::addEventHandler(const EventHandlerPtr& eventHandler)
    {
        std::vector<EventHandlerPtr>::iterator i = std::find(eventHandlers.begin(), eventHandlers.end(), eventHandler);

        if (i == eventHandlers.end())
        {
            eventHandler->remove = false;
            eventHandlers.push_back(eventHandler);

            std::sort(eventHandlers.begin(), eventHandlers.end(), [](const EventHandlerPtr& a, const EventHandlerPtr& b) {
                return a->priority < b->priority;
            });
        }
    }

    void EventDispatcher::removeEventHandler(const EventHandlerPtr& eventHandler)
    {
        std::vector<EventHandlerPtr>::iterator i = std::find(eventHandlers.begin(), eventHandlers.end(), eventHandler);

        if (i != eventHandlers.end())
        {
            eventHandlers.erase(i);
        }
    }

    void EventDispatcher::dispatchEvent(const Event& event)
    {
        std::lock_guard<std::mutex> mutexLock(mutex);

        eventQueue.push(event);
    }
}
