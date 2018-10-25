// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include "core/Setup.h"
#include "EventDispatcher.hpp"
#include "EventHandler.hpp"
#include "utils/Utils.hpp"

namespace ouzel
{
    EventDispatcher::EventDispatcher()
    {
    }

    EventDispatcher::~EventDispatcher()
    {
        for (EventHandler* eventHandler : eventHandlerAddSet)
            eventHandler->eventDispatcher = nullptr;

        for (EventHandler* eventHandler : eventHandlers)
        {
            auto i = std::find(eventHandlerDeleteSet.begin(), eventHandlerDeleteSet.end(), eventHandler);
            if (i == eventHandlerDeleteSet.end()) eventHandler->eventDispatcher = nullptr;
        }
    }

    void EventDispatcher::dispatchEvents()
    {
        for (EventHandler* eventHandler : eventHandlerDeleteSet)
        {
            auto i = std::find(eventHandlers.begin(), eventHandlers.end(), eventHandler);

            if (i != eventHandlers.end())
                eventHandlers.erase(i);
        }

        eventHandlerDeleteSet.clear();

        for (EventHandler* eventHandler : eventHandlerAddSet)
        {
            auto i = std::find(eventHandlers.begin(), eventHandlers.end(), eventHandler);

            if (i == eventHandlers.end())
            {
                auto upperBound = std::upper_bound(eventHandlers.begin(), eventHandlers.end(), eventHandler,
                                                   [](const EventHandler* a, const EventHandler* b) {
                                                       return a->priority > b->priority;
                                                   });

                eventHandlers.insert(upperBound, eventHandler);
            }
        }

        eventHandlerAddSet.clear();

        std::pair<std::promise<bool>, Event> event;

        for (;;)
        {
            {
                std::unique_lock<std::mutex> lock(eventQueueMutex);
                if (eventQueue.empty()) break;

                event = std::move(eventQueue.front());
                eventQueue.pop();
            }

            event.first.set_value(dispatchEvent(event.second));
        }
    }

    bool EventDispatcher::dispatchEvent(const Event& event)
    {
        bool handled = false;

        for (const EventHandler* eventHandler : eventHandlers)
        {
            auto i = std::find(eventHandlerDeleteSet.begin(), eventHandlerDeleteSet.end(), eventHandler);

            if (i == eventHandlerDeleteSet.end())
            {
                switch (event.type)
                {
                    case Event::Type::KEYBOARD_CONNECT:
                    case Event::Type::KEYBOARD_DISCONNECT:
                    case Event::Type::KEY_PRESS:
                    case Event::Type::KEY_RELEASE:
                    case Event::Type::KEY_REPEAT:
                        if (eventHandler->keyboardHandler)
                            handled = eventHandler->keyboardHandler(event.type, event.keyboardEvent);
                        break;

                    case Event::Type::MOUSE_CONNECT:
                    case Event::Type::MOUSE_DISCONNECT:
                    case Event::Type::MOUSE_PRESS:
                    case Event::Type::MOUSE_RELEASE:
                    case Event::Type::MOUSE_SCROLL:
                    case Event::Type::MOUSE_MOVE:
                    case Event::Type::MOUSE_CURSOR_LOCK_CHANGE:
                        if (eventHandler->mouseHandler)
                            handled = eventHandler->mouseHandler(event.type, event.mouseEvent);
                        break;
                    case Event::Type::TOUCHPAD_CONNECT:
                    case Event::Type::TOUCHPAD_DISCONNECT:
                    case Event::Type::TOUCH_BEGIN:
                    case Event::Type::TOUCH_MOVE:
                    case Event::Type::TOUCH_END:
                    case Event::Type::TOUCH_CANCEL:
                        if (eventHandler->touchHandler)
                            handled = eventHandler->touchHandler(event.type, event.touchEvent);
                        break;
                    case Event::Type::GAMEPAD_CONNECT:
                    case Event::Type::GAMEPAD_DISCONNECT:
                    case Event::Type::GAMEPAD_BUTTON_CHANGE:
                        if (eventHandler->gamepadHandler)
                            handled = eventHandler->gamepadHandler(event.type, event.gamepadEvent);
                        break;
                    case Event::Type::WINDOW_SIZE_CHANGE:
                    case Event::Type::WINDOW_TITLE_CHANGE:
                    case Event::Type::FULLSCREEN_CHANGE:
                    case Event::Type::SCREEN_CHANGE:
                    case Event::Type::RESOLUTION_CHANGE:
                        if (eventHandler->windowHandler)
                            handled = eventHandler->windowHandler(event.type, event.windowEvent);
                        break;
                    case Event::Type::ENGINE_START:
                    case Event::Type::ENGINE_STOP:
                    case Event::Type::ENGINE_RESUME:
                    case Event::Type::ENGINE_PAUSE:
                    case Event::Type::ORIENTATION_CHANGE:
                    case Event::Type::LOW_MEMORY:
                    case Event::Type::OPEN_FILE:
                        if (eventHandler->systemHandler)
                            handled = eventHandler->systemHandler(event.type, event.systemEvent);
                        break;
                    case Event::Type::ACTOR_ENTER:
                    case Event::Type::ACTOR_LEAVE:
                    case Event::Type::ACTOR_PRESS:
                    case Event::Type::ACTOR_RELEASE:
                    case Event::Type::ACTOR_CLICK:
                    case Event::Type::ACTOR_DRAG:
                    case Event::Type::WIDGET_CHANGE:
                        if (eventHandler->uiHandler)
                            handled = eventHandler->uiHandler(event.type, event.uiEvent);
                        break;
                    case Event::Type::ANIMATION_START:
                    case Event::Type::ANIMATION_RESET:
                    case Event::Type::ANIMATION_FINISH:
                        if (eventHandler->animationHandler)
                            handled = eventHandler->animationHandler(event.type, event.animationEvent);
                        break;
                    case Event::Type::SOUND_START:
                    case Event::Type::SOUND_RESET:
                    case Event::Type::SOUND_FINISH:
                        if (eventHandler->soundHandler)
                            handled = eventHandler->soundHandler(event.type, event.soundEvent);
                        break;
                    case Event::Type::UPDATE:
                        if (eventHandler->updateHandler)
                            handled = eventHandler->updateHandler(event.type, event.updateEvent);
                        break;
                    case Event::Type::USER:
                        if (eventHandler->userHandler)
                            handled = eventHandler->userHandler(event.type, event.userEvent);
                        break;
                    default:
                        return false; // custom event should not be sent
                }
            }

            if (handled) break;
        }

        return handled;
    }

    void EventDispatcher::addEventHandler(EventHandler* eventHandler)
    {
        if (eventHandler->eventDispatcher)
            eventHandler->eventDispatcher->removeEventHandler(eventHandler);

        eventHandler->eventDispatcher = this;

        eventHandlerAddSet.insert(eventHandler);

        auto setIterator = eventHandlerDeleteSet.find(eventHandler);

        if (setIterator != eventHandlerDeleteSet.end())
            eventHandlerDeleteSet.erase(setIterator);
    }

    void EventDispatcher::removeEventHandler(EventHandler* eventHandler)
    {
        if (eventHandler->eventDispatcher == this)
            eventHandler->eventDispatcher = nullptr;

        eventHandlerDeleteSet.insert(eventHandler);

        auto setIterator = eventHandlerAddSet.find(eventHandler);

        if (setIterator != eventHandlerAddSet.end())
            eventHandlerAddSet.erase(setIterator);
    }

    std::future<bool> EventDispatcher::postEvent(const Event& event)
    {
        std::future<bool> future;

#if OUZEL_MULTITHREADED
        std::unique_lock<std::mutex> lock(eventQueueMutex);
        std::promise<bool> promise;
        future = promise.get_future();
        eventQueue.push(std::pair<std::promise<bool>, Event>(std::move(promise), event));
#else
        std::promise<bool> promise;
        future = promise.get_future();
        promise.set_value(dispatchEvent(event));
#endif

        return future;
    }
}
