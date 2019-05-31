// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <algorithm>
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

        std::pair<std::promise<bool>, std::unique_ptr<Event>> event;

        for (;;)
        {
            std::unique_lock<std::mutex> lock(eventQueueMutex);
            if (eventQueue.empty()) break;

            event = std::move(eventQueue.front());
            eventQueue.pop();
            lock.unlock();

            event.first.set_value(dispatchEvent(std::move(event.second)));
        }
    }

    bool EventDispatcher::dispatchEvent(std::unique_ptr<Event>&& event)
    {
        if (!event) return false;

        bool handled = false;

        for (const EventHandler* eventHandler : eventHandlers)
        {
            auto i = std::find(eventHandlerDeleteSet.begin(), eventHandlerDeleteSet.end(), eventHandler);

            if (i == eventHandlerDeleteSet.end())
            {
                switch (event->type)
                {
                    case Event::Type::KEYBOARD_CONNECT:
                    case Event::Type::KEYBOARD_DISCONNECT:
                    case Event::Type::KEY_PRESS:
                    case Event::Type::KEY_RELEASE:
                        if (eventHandler->keyboardHandler)
                            handled = eventHandler->keyboardHandler(*static_cast<KeyboardEvent*>(event.get()));
                        break;
                    case Event::Type::MOUSE_CONNECT:
                    case Event::Type::MOUSE_DISCONNECT:
                    case Event::Type::MOUSE_PRESS:
                    case Event::Type::MOUSE_RELEASE:
                    case Event::Type::MOUSE_SCROLL:
                    case Event::Type::MOUSE_MOVE:
                    case Event::Type::MOUSE_CURSOR_LOCK_CHANGE:
                        if (eventHandler->mouseHandler)
                            handled = eventHandler->mouseHandler(*static_cast<MouseEvent*>(event.get()));
                        break;
                    case Event::Type::TOUCHPAD_CONNECT:
                    case Event::Type::TOUCHPAD_DISCONNECT:
                    case Event::Type::TOUCH_BEGIN:
                    case Event::Type::TOUCH_MOVE:
                    case Event::Type::TOUCH_END:
                    case Event::Type::TOUCH_CANCEL:
                        if (eventHandler->touchHandler)
                            handled = eventHandler->touchHandler(*static_cast<TouchEvent*>(event.get()));
                        break;
                    case Event::Type::GAMEPAD_CONNECT:
                    case Event::Type::GAMEPAD_DISCONNECT:
                    case Event::Type::GAMEPAD_BUTTON_CHANGE:
                        if (eventHandler->gamepadHandler)
                            handled = eventHandler->gamepadHandler(*static_cast<GamepadEvent*>(event.get()));
                        break;
                    case Event::Type::WINDOW_SIZE_CHANGE:
                    case Event::Type::WINDOW_TITLE_CHANGE:
                    case Event::Type::FULLSCREEN_CHANGE:
                    case Event::Type::SCREEN_CHANGE:
                    case Event::Type::RESOLUTION_CHANGE:
                        if (eventHandler->windowHandler)
                            handled = eventHandler->windowHandler(*static_cast<WindowEvent*>(event.get()));
                        break;
                    case Event::Type::ENGINE_START:
                    case Event::Type::ENGINE_STOP:
                    case Event::Type::ENGINE_RESUME:
                    case Event::Type::ENGINE_PAUSE:
                    case Event::Type::ORIENTATION_CHANGE:
                    case Event::Type::LOW_MEMORY:
                    case Event::Type::OPEN_FILE:
                        if (eventHandler->systemHandler)
                            handled = eventHandler->systemHandler(*static_cast<SystemEvent*>(event.get()));
                        break;
                    case Event::Type::ACTOR_ENTER:
                    case Event::Type::ACTOR_LEAVE:
                    case Event::Type::ACTOR_PRESS:
                    case Event::Type::ACTOR_RELEASE:
                    case Event::Type::ACTOR_CLICK:
                    case Event::Type::ACTOR_DRAG:
                    case Event::Type::WIDGET_CHANGE:
                        if (eventHandler->uiHandler)
                            handled = eventHandler->uiHandler(*static_cast<UIEvent*>(event.get()));
                        break;
                    case Event::Type::ANIMATION_START:
                    case Event::Type::ANIMATION_RESET:
                    case Event::Type::ANIMATION_FINISH:
                        if (eventHandler->animationHandler)
                            handled = eventHandler->animationHandler(*static_cast<AnimationEvent*>(event.get()));
                        break;
                    case Event::Type::SOUND_START:
                    case Event::Type::SOUND_RESET:
                    case Event::Type::SOUND_FINISH:
                        if (eventHandler->soundHandler)
                            handled = eventHandler->soundHandler(*static_cast<SoundEvent*>(event.get()));
                        break;
                    case Event::Type::UPDATE:
                        if (eventHandler->updateHandler)
                            handled = eventHandler->updateHandler(*static_cast<UpdateEvent*>(event.get()));
                        break;
                    case Event::Type::USER:
                        if (eventHandler->userHandler)
                            handled = eventHandler->userHandler(*static_cast<UserEvent*>(event.get()));
                        break;
                    default:
                        return false; // custom event should not be sent
                }
            }

            if (handled) break;
        }

        return handled;
    }

    void EventDispatcher::addEventHandler(EventHandler& eventHandler)
    {
        if (eventHandler.eventDispatcher)
            eventHandler.eventDispatcher->removeEventHandler(eventHandler);

        eventHandler.eventDispatcher = this;

        eventHandlerAddSet.insert(&eventHandler);

        auto setIterator = eventHandlerDeleteSet.find(&eventHandler);

        if (setIterator != eventHandlerDeleteSet.end())
            eventHandlerDeleteSet.erase(setIterator);
    }

    void EventDispatcher::removeEventHandler(EventHandler& eventHandler)
    {
        if (eventHandler.eventDispatcher == this)
            eventHandler.eventDispatcher = nullptr;

        eventHandlerDeleteSet.insert(&eventHandler);

        auto setIterator = eventHandlerAddSet.find(&eventHandler);

        if (setIterator != eventHandlerAddSet.end())
            eventHandlerAddSet.erase(setIterator);
    }

    std::future<bool> EventDispatcher::postEvent(std::unique_ptr<Event>&& event)
    {
        std::promise<bool> promise;
        std::future<bool> future = promise.get_future();

#if defined(__EMSCRIPTEN__)
        promise.set_value(dispatchEvent(std::move(event)));
#else
        std::unique_lock<std::mutex> lock(eventQueueMutex);
        eventQueue.push(std::pair<std::promise<bool>, std::unique_ptr<Event>>(std::move(promise), std::move(event)));
#endif

        return future;
    }
}
