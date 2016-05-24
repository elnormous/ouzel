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

        while (!eventQueue.empty())
        {
            mutex.lock();

            auto eventPair = eventQueue.front();
            eventQueue.pop();

            mutex.unlock();

            for (const EventHandlerPtr& eventHandler : eventHandlersCopy)
            {
                if (eventHandler)
                {
                    switch (eventPair.first->type)
                    {
                        case Event::Type::KEY_DOWN:
                        case Event::Type::KEY_UP:
                        case Event::Type::KEY_REPEAT:
                            if (eventHandler->keyboardHandler)
                            {
                                eventHandler->keyboardHandler(std::static_pointer_cast<KeyboardEvent>(eventPair.first), eventPair.second);
                            }
                            break;

                        case Event::Type::MOUSE_DOWN:
                        case Event::Type::MOUSE_UP:
                        case Event::Type::MOUSE_SCROLL:
                        case Event::Type::MOUSE_MOVE:
                            if (eventHandler->mouseHandler)
                            {
                                eventHandler->mouseHandler(std::static_pointer_cast<MouseEvent>(eventPair.first), eventPair.second);
                            }
                            break;
                        case Event::Type::TOUCH_BEGIN:
                        case Event::Type::TOUCH_MOVE:
                        case Event::Type::TOUCH_END:
                        case Event::Type::TOUCH_CANCEL:
                            if (eventHandler->touchHandler)
                            {
                                eventHandler->touchHandler(std::static_pointer_cast<TouchEvent>(eventPair.first), eventPair.second);
                            }
                            break;
                        case Event::Type::GAMEPAD_CONNECT:
                        case Event::Type::GAMEPAD_DISCONNECT:
                        case Event::Type::GAMEPAD_BUTTON_CHANGE:
                            if (eventHandler->gamepadHandler)
                            {
                                eventHandler->gamepadHandler(std::static_pointer_cast<GamepadEvent>(eventPair.first), eventPair.second);
                            }
                            break;
                        case Event::Type::WINDOW_SIZE_CHANGE:
                        case Event::Type::WINDOW_TITLE_CHANGE:
                        case Event::Type::WINDOW_FULLSCREEN_CHANGE:
                            if (eventHandler->windowHandler)
                            {
                                eventHandler->windowHandler(std::static_pointer_cast<WindowEvent>(eventPair.first), eventPair.second);
                            }
                            break;
                        case Event::Type::LOW_MEMORY:
                        case Event::Type::OPEN_FILE:
                            if (eventHandler->systemHandler)
                            {
                                eventHandler->systemHandler(std::static_pointer_cast<SystemEvent>(eventPair.first), eventPair.second);
                            }
                            break;
                        case Event::Type::UI_ENTER_NODE:
                        case Event::Type::UI_LEAVE_NODE:
                        case Event::Type::UI_PRESS_NODE:
                        case Event::Type::UI_RELEASE_NODE:
                        case Event::Type::UI_CLICK_NODE:
                        case Event::Type::UI_DRAG_NODE:
                            if (eventHandler->uiHandler)
                            {
                                eventHandler->uiHandler(std::static_pointer_cast<UIEvent>(eventPair.first), eventPair.second);
                            }
                            break;
                    }
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

    void EventDispatcher::dispatchEvent(const EventPtr& event, const VoidPtr& sender)
    {
        std::lock_guard<std::mutex> mutexLock(mutex);

        eventQueue.push(std::make_pair(event, sender));
    }
}
