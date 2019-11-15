// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include "EventDispatcher.hpp"
#include "EventHandler.hpp"
#include "utils/Utils.hpp"

namespace ouzel
{
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
                                                   [](const EventHandler* a, const EventHandler* b) noexcept {
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

    bool EventDispatcher::dispatchEvent(std::unique_ptr<Event> event)
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
                    case Event::Type::KeyboardConnect:
                    case Event::Type::KeyboardDisconnect:
                    case Event::Type::KeyboardKeyPress:
                    case Event::Type::KeyboardKeyRelease:
                        if (eventHandler->keyboardHandler)
                            handled = eventHandler->keyboardHandler(*static_cast<KeyboardEvent*>(event.get()));
                        break;
                    case Event::Type::MouseConnect:
                    case Event::Type::MouseDisconnect:
                    case Event::Type::MousePress:
                    case Event::Type::MouseRelease:
                    case Event::Type::MouseScroll:
                    case Event::Type::MouseMove:
                    case Event::Type::MouseCursorLockChange:
                        if (eventHandler->mouseHandler)
                            handled = eventHandler->mouseHandler(*static_cast<MouseEvent*>(event.get()));
                        break;
                    case Event::Type::TouchpadConnect:
                    case Event::Type::TouchpadDisconnect:
                    case Event::Type::TouchBegin:
                    case Event::Type::TouchMove:
                    case Event::Type::TouchEnd:
                    case Event::Type::TouchCancel:
                        if (eventHandler->touchHandler)
                            handled = eventHandler->touchHandler(*static_cast<TouchEvent*>(event.get()));
                        break;
                    case Event::Type::GamepadConnect:
                    case Event::Type::GamepadDisconnect:
                    case Event::Type::GamepadButtonChange:
                        if (eventHandler->gamepadHandler)
                            handled = eventHandler->gamepadHandler(*static_cast<GamepadEvent*>(event.get()));
                        break;
                    case Event::Type::WindowSizeChange:
                    case Event::Type::WindowTitleChange:
                    case Event::Type::FullscreenChange:
                    case Event::Type::ScreenChange:
                    case Event::Type::ResolutionChange:
                        if (eventHandler->windowHandler)
                            handled = eventHandler->windowHandler(*static_cast<WindowEvent*>(event.get()));
                        break;
                    case Event::Type::EngineStart:
                    case Event::Type::EngineStop:
                    case Event::Type::EngineResume:
                    case Event::Type::EnginePause:
                    case Event::Type::OrientationChange:
                    case Event::Type::LowMemory:
                    case Event::Type::OpenFile:
                        if (eventHandler->systemHandler)
                            handled = eventHandler->systemHandler(*static_cast<SystemEvent*>(event.get()));
                        break;
                    case Event::Type::ActorEnter:
                    case Event::Type::ActorLeave:
                    case Event::Type::ActorPress:
                    case Event::Type::ActorRelease:
                    case Event::Type::ActorClick:
                    case Event::Type::ActorDrag:
                    case Event::Type::WidgetChange:
                        if (eventHandler->uiHandler)
                            handled = eventHandler->uiHandler(*static_cast<UIEvent*>(event.get()));
                        break;
                    case Event::Type::AnimationStart:
                    case Event::Type::AnimationReset:
                    case Event::Type::AnimationFinish:
                        if (eventHandler->animationHandler)
                            handled = eventHandler->animationHandler(*static_cast<AnimationEvent*>(event.get()));
                        break;
                    case Event::Type::SoundStart:
                    case Event::Type::SoundReset:
                    case Event::Type::SoundFinish:
                        if (eventHandler->soundHandler)
                            handled = eventHandler->soundHandler(*static_cast<SoundEvent*>(event.get()));
                        break;
                    case Event::Type::Update:
                        if (eventHandler->updateHandler)
                            handled = eventHandler->updateHandler(*static_cast<UpdateEvent*>(event.get()));
                        break;
                    case Event::Type::User:
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

    std::future<bool> EventDispatcher::postEvent(std::unique_ptr<Event> event)
    {
        std::promise<bool> promise;
        std::future<bool> future = promise.get_future();

#if defined(__EMSCRIPTEN__)
        promise.set_value(dispatchEvent(std::move(event)));
#else
        std::lock_guard<std::mutex> lock(eventQueueMutex);
        eventQueue.push(std::pair<std::promise<bool>, std::unique_ptr<Event>>(std::move(promise), std::move(event)));
#endif

        return future;
    }
}
