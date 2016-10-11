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

    void EventDispatcher::dispatchEvents()
    {
        // erase all null event handlers from the list
        for (auto i = eventHandlers.begin(); i != eventHandlers.end();)
        {
            i = (*i) ? ++i : eventHandlers.erase(i);
        }

        if (!eventHandlerAddSet.empty())
        {
            for (const EventHandler* eventHandler : eventHandlerAddSet)
            {
                auto i = std::find(eventHandlers.begin(), eventHandlers.end(), eventHandler);

                if (i == eventHandlers.end())
                {
                    eventHandlers.push_back(eventHandler);
                }
            }

            std::stable_sort(eventHandlers.begin(), eventHandlers.end(), [](const EventHandler* a, const EventHandler* b) {
                return a->priority > b->priority;
            });
        }

        Event event;

        for (;;)
        {
            {
                std::lock_guard<std::mutex> lock(eventQueueMutex);
                if (eventQueue.empty())
                {
                    break;
                }

                event = eventQueue.front();
                eventQueue.pop();
            }

            bool propagate = true;

            for (const EventHandler* eventHandler : eventHandlers)
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
                                propagate = eventHandler->keyboardHandler(event.type, event.keyboardEvent);
                            }
                            break;

                        case Event::Type::MOUSE_DOWN:
                        case Event::Type::MOUSE_UP:
                        case Event::Type::MOUSE_SCROLL:
                        case Event::Type::MOUSE_MOVE:
                            if (eventHandler->mouseHandler)
                            {
                                propagate = eventHandler->mouseHandler(event.type, event.mouseEvent);
                            }
                            break;
                        case Event::Type::TOUCH_BEGIN:
                        case Event::Type::TOUCH_MOVE:
                        case Event::Type::TOUCH_END:
                        case Event::Type::TOUCH_CANCEL:
                            if (eventHandler->touchHandler)
                            {
                                propagate = eventHandler->touchHandler(event.type, event.touchEvent);
                            }
                            break;
                        case Event::Type::GAMEPAD_CONNECT:
                        case Event::Type::GAMEPAD_DISCONNECT:
                        case Event::Type::GAMEPAD_BUTTON_CHANGE:
                            if (eventHandler->gamepadHandler)
                            {
                                propagate = eventHandler->gamepadHandler(event.type, event.gamepadEvent);
                            }
                            break;
                        case Event::Type::WINDOW_SIZE_CHANGE:
                        case Event::Type::WINDOW_TITLE_CHANGE:
                        case Event::Type::WINDOW_FULLSCREEN_CHANGE:
                        case Event::Type::WINDOW_RESOLUTION_CHANGE:
                            if (eventHandler->windowHandler)
                            {
                                propagate = eventHandler->windowHandler(event.type, event.windowEvent);
                            }
                            break;
                        case Event::Type::ORIENTATION_CHANGE:
                        case Event::Type::LOW_MEMORY:
                        case Event::Type::OPEN_FILE:
                            if (eventHandler->systemHandler)
                            {
                                propagate = eventHandler->systemHandler(event.type, event.systemEvent);
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
                                propagate = eventHandler->uiHandler(event.type, event.uiEvent);
                            }
                            break;
                        case Event::Type::USER:
                            if (eventHandler->userHandler)
                            {
                                propagate = eventHandler->userHandler(event.type, event.userEvent);
                            }
                            break;
                    }
                }
            }

            if (!propagate)
            {
                break;
            }
        }
    }

    void EventDispatcher::addEventHandler(const EventHandler& eventHandler)
    {
        eventHandlerAddSet.insert(&eventHandler);
    }

    void EventDispatcher::removeEventHandler(const EventHandler& eventHandler)
    {
        auto vectorIterator = std::find(eventHandlers.begin(), eventHandlers.end(), &eventHandler);

        if (vectorIterator != eventHandlers.end())
        {
            *vectorIterator = nullptr;
        }

        auto setIterator = eventHandlerAddSet.find(&eventHandler);

        if (setIterator != eventHandlerAddSet.end())
        {
            eventHandlerAddSet.erase(setIterator);
        }
    }

    void EventDispatcher::postEvent(const Event& event)
    {
        std::lock_guard<std::mutex> lock(eventQueueMutex);

        eventQueue.push(event);
    }
}
