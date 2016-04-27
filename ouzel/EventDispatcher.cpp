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
        lock();

        while (!eventQueue.empty())
        {
            mutex.lock();

            auto eventPair = eventQueue.front();
            eventQueue.pop();

            mutex.unlock();

            switch (eventPair.first->type)
            {
                case Event::Type::KEY_DOWN:
                case Event::Type::KEY_UP:
                case Event::Type::KEY_REPEAT:
                    dispatchKeyboardEvent(std::static_pointer_cast<KeyboardEvent>(eventPair.first), eventPair.second);
                    break;

                case Event::Type::MOUSE_DOWN:
                case Event::Type::MOUSE_UP:
                case Event::Type::MOUSE_SCROLL:
                case Event::Type::MOUSE_MOVE:
                    dispatchMouseEvent(std::static_pointer_cast<MouseEvent>(eventPair.first), eventPair.second);
                    break;
                case Event::Type::TOUCH_BEGIN:
                case Event::Type::TOUCH_MOVE:
                case Event::Type::TOUCH_END:
                case Event::Type::TOUCH_CANCEL:
                    dispatchTouchEvent(std::static_pointer_cast<TouchEvent>(eventPair.first), eventPair.second);
                    break;
                case Event::Type::GAMEPAD_CONNECT:
                case Event::Type::GAMEPAD_DISCONNECT:
                case Event::Type::GAMEPAD_BUTTON_CHANGE:
                    dispatchGamepadEvent(std::static_pointer_cast<GamepadEvent>(eventPair.first), eventPair.second);
                    break;
                case Event::Type::WINDOW_SIZE_CHANGE:
                case Event::Type::WINDOW_TITLE_CHANGE:
                case Event::Type::WINDOW_FULLSCREEN_CHANGE:
                    dispatchWindowEvent(std::static_pointer_cast<WindowEvent>(eventPair.first), eventPair.second);
                    break;
                case Event::Type::LOW_MEMORY:
                case Event::Type::OPEN_FILE:
                    dispatchSystemEvent(std::static_pointer_cast<SystemEvent>(eventPair.first), eventPair.second);
                    break;
                case Event::Type::UI_ENTER_NODE:
                case Event::Type::UI_LEAVE_NODE:
                case Event::Type::UI_PRESS_NODE:
                case Event::Type::UI_RELEASE_NODE:
                case Event::Type::UI_CLICK_NODE:
                case Event::Type::UI_DRAG_NODE:
                    dispatchUIEvent(std::static_pointer_cast<UIEvent>(eventPair.first), eventPair.second);
                    break;
            }
        }

        unlock();
    }

    void EventDispatcher::addEventHandler(const EventHandlerPtr& eventHandler)
    {
        if (locked)
        {
            eventHandlerAddList.insert(eventHandler);
        }
        else
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
    }

    void EventDispatcher::removeEventHandler(const EventHandlerPtr& eventHandler)
    {
        if (locked)
        {
            eventHandler->remove = true;
            eventHandlerRemoveList.insert(eventHandler);
        }
        else
        {
            std::vector<EventHandlerPtr>::iterator i = std::find(eventHandlers.begin(), eventHandlers.end(), eventHandler);

            if (i != eventHandlers.end())
            {
                eventHandlers.erase(i);
            }
        }
    }

    void EventDispatcher::dispatchEvent(const EventPtr& event, const VoidPtr& sender)
    {
        std::lock_guard<std::mutex> mutexLock(mutex);

        eventQueue.push(std::make_pair(event, sender));
    }

    void EventDispatcher::dispatchKeyboardEvent(const KeyboardEventPtr& event, const VoidPtr& sender)
    {
        lock();

        for (const EventHandlerPtr& eventHandler : eventHandlers)
        {
            if (eventHandler && !eventHandler->remove && eventHandler->keyboardHandler)
            {
                if (!eventHandler->keyboardHandler(event, sender))
                {
                    break;
                }
            }
        }

        unlock();
    }

    void EventDispatcher::dispatchMouseEvent(const MouseEventPtr& event, const VoidPtr& sender)
    {
        lock();

        for (const EventHandlerPtr& eventHandler : eventHandlers)
        {
            if (eventHandler && !eventHandler->remove && eventHandler->mouseHandler)
            {
                if (!eventHandler->mouseHandler(event, sender))
                {
                    break;
                }
            }
        }

        unlock();
    }

    void EventDispatcher::dispatchTouchEvent(const TouchEventPtr& event, const VoidPtr& sender)
    {
        lock();

        for (const EventHandlerPtr& eventHandler : eventHandlers)
        {
            if (eventHandler && !eventHandler->remove && eventHandler->touchHandler)
            {
                if (!eventHandler->touchHandler(event, sender))
                {
                    break;
                }
            }
        }

        unlock();
    }

    void EventDispatcher::dispatchGamepadEvent(const GamepadEventPtr& event, const VoidPtr& sender)
    {
        lock();

        for (const EventHandlerPtr& eventHandler : eventHandlers)
        {
            if (eventHandler && !eventHandler->remove && eventHandler->gamepadHandler)
            {
                if (!eventHandler->gamepadHandler(event, sender))
                {
                    break;
                }
            }
        }

        unlock();
    }

    void EventDispatcher::dispatchWindowEvent(const WindowEventPtr& event, const VoidPtr& sender)
    {
        lock();

        for (const EventHandlerPtr& eventHandler : eventHandlers)
        {
            if (eventHandler && !eventHandler->remove && eventHandler->windowHandler)
            {
                if (!eventHandler->windowHandler(event, sender))
                {
                    break;
                }
            }
        }

        unlock();
    }

    void EventDispatcher::dispatchSystemEvent(const SystemEventPtr& event, const VoidPtr& sender)
    {
        lock();

        for (const EventHandlerPtr& eventHandler : eventHandlers)
        {
            if (eventHandler && !eventHandler->remove && eventHandler->systemHandler)
            {
                if (!eventHandler->systemHandler(event, sender))
                {
                    break;
                }
            }
        }

        unlock();
    }

    void EventDispatcher::dispatchUIEvent(const UIEventPtr& event, const VoidPtr& sender)
    {
        lock();

        for (const EventHandlerPtr& eventHandler : eventHandlers)
        {
            if (eventHandler && !eventHandler->remove && eventHandler->uiHandler)
            {
                if (!eventHandler->uiHandler(event, sender))
                {
                    break;
                }
            }
        }

        unlock();
    }

    void EventDispatcher::lock()
    {
        ++locked;
    }

    void EventDispatcher::unlock()
    {
        if (--locked == 0)
        {
            if (!eventHandlerAddList.empty())
            {
                for (const EventHandlerPtr& eventHandler : eventHandlerAddList)
                {
                    addEventHandler(eventHandler);
                }
                eventHandlerAddList.clear();
            }

            if (!eventHandlerRemoveList.empty())
            {
                for (const EventHandlerPtr& eventHandler : eventHandlerRemoveList)
                {
                    removeEventHandler(eventHandler);
                }
                eventHandlerRemoveList.clear();
            }
        }
    }
}
