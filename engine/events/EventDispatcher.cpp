// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include "EventDispatcher.hpp"
#include "EventHandler.hpp"
#include "../utils/Utils.hpp"

namespace ouzel
{
    EventDispatcher::~EventDispatcher()
    {
        for (EventHandler* eventHandler : eventHandlerAddSet)
            eventHandler->eventDispatcher = nullptr;

        for (EventHandler* eventHandler : eventHandlers)
        {
            const auto i = std::find(eventHandlerDeleteSet.begin(),
                                     eventHandlerDeleteSet.end(),
                                     eventHandler);
            if (i == eventHandlerDeleteSet.end()) eventHandler->eventDispatcher = nullptr;
        }
    }

    void EventDispatcher::dispatchEvents()
    {
        for (EventHandler* eventHandler : eventHandlerDeleteSet)
        {
            const auto i = std::find(eventHandlers.begin(),
                                     eventHandlers.end(),
                                     eventHandler);

            if (i != eventHandlers.end())
                eventHandlers.erase(i);
        }

        eventHandlerDeleteSet.clear();

        for (EventHandler* eventHandler : eventHandlerAddSet)
        {
            const auto i = std::find(eventHandlers.begin(),
                                     eventHandlers.end(),
                                     eventHandler);

            if (i == eventHandlers.end())
            {
                const auto upperBound = std::upper_bound(eventHandlers.begin(), eventHandlers.end(), eventHandler,
                                                         [](const auto a, const auto b) noexcept {
                                                                return a->priority > b->priority;
                                                            });

                eventHandlers.insert(upperBound, eventHandler);
            }
        }

        eventHandlerAddSet.clear();

        std::pair<std::promise<bool>, std::unique_ptr<Event>> event;

        for (;;)
        {
            std::unique_lock lock(eventQueueMutex);
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

        for (const auto eventHandler : eventHandlers)
        {
            const auto i = std::find(eventHandlerDeleteSet.begin(),
                                     eventHandlerDeleteSet.end(),
                                     eventHandler);

            if (i == eventHandlerDeleteSet.end())
            {
                switch (event->type)
                {
                    case Event::Type::keyboardConnect:
                    case Event::Type::keyboardDisconnect:
                    case Event::Type::keyboardKeyPress:
                    case Event::Type::keyboardKeyRelease:
                        if (eventHandler->keyboardHandler)
                            handled = eventHandler->keyboardHandler(*static_cast<KeyboardEvent*>(event.get()));
                        break;
                    case Event::Type::mouseConnect:
                    case Event::Type::mouseDisconnect:
                    case Event::Type::mousePress:
                    case Event::Type::mouseRelease:
                    case Event::Type::mouseScroll:
                    case Event::Type::mouseMove:
                    case Event::Type::mouseCursorLockChange:
                        if (eventHandler->mouseHandler)
                            handled = eventHandler->mouseHandler(*static_cast<MouseEvent*>(event.get()));
                        break;
                    case Event::Type::touchpadConnect:
                    case Event::Type::touchpadDisconnect:
                    case Event::Type::touchBegin:
                    case Event::Type::touchMove:
                    case Event::Type::touchEnd:
                    case Event::Type::touchCancel:
                        if (eventHandler->touchHandler)
                            handled = eventHandler->touchHandler(*static_cast<TouchEvent*>(event.get()));
                        break;
                    case Event::Type::gamepadConnect:
                    case Event::Type::gamepadDisconnect:
                    case Event::Type::gamepadButtonChange:
                        if (eventHandler->gamepadHandler)
                            handled = eventHandler->gamepadHandler(*static_cast<GamepadEvent*>(event.get()));
                        break;
                    case Event::Type::windowSizeChange:
                    case Event::Type::windowTitleChange:
                    case Event::Type::fullscreenChange:
                    case Event::Type::screenChange:
                    case Event::Type::resolutionChange:
                        if (eventHandler->windowHandler)
                            handled = eventHandler->windowHandler(*static_cast<WindowEvent*>(event.get()));
                        break;
                    case Event::Type::engineStart:
                    case Event::Type::engineStop:
                    case Event::Type::engineResume:
                    case Event::Type::enginePause:
                    case Event::Type::orientationChange:
                    case Event::Type::lowMemory:
                    case Event::Type::openFile:
                        if (eventHandler->systemHandler)
                            handled = eventHandler->systemHandler(*static_cast<SystemEvent*>(event.get()));
                        break;
                    case Event::Type::actorEnter:
                    case Event::Type::actorLeave:
                    case Event::Type::actorPress:
                    case Event::Type::actorRelease:
                    case Event::Type::actorClick:
                    case Event::Type::actorDrag:
                    case Event::Type::widgetChange:
                        if (eventHandler->uiHandler)
                            handled = eventHandler->uiHandler(*static_cast<UIEvent*>(event.get()));
                        break;
                    case Event::Type::animationStart:
                    case Event::Type::animationReset:
                    case Event::Type::animationFinish:
                        if (eventHandler->animationHandler)
                            handled = eventHandler->animationHandler(*static_cast<AnimationEvent*>(event.get()));
                        break;
                    case Event::Type::soundStart:
                    case Event::Type::soundReset:
                    case Event::Type::soundFinish:
                        if (eventHandler->soundHandler)
                            handled = eventHandler->soundHandler(*static_cast<SoundEvent*>(event.get()));
                        break;
                    case Event::Type::update:
                        if (eventHandler->updateHandler)
                            handled = eventHandler->updateHandler(*static_cast<UpdateEvent*>(event.get()));
                        break;
                    case Event::Type::user:
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

        const auto setIterator = eventHandlerDeleteSet.find(&eventHandler);

        if (setIterator != eventHandlerDeleteSet.end())
            eventHandlerDeleteSet.erase(setIterator);
    }

    void EventDispatcher::removeEventHandler(EventHandler& eventHandler)
    {
        if (eventHandler.eventDispatcher == this)
            eventHandler.eventDispatcher = nullptr;

        eventHandlerDeleteSet.insert(&eventHandler);

        const auto setIterator = eventHandlerAddSet.find(&eventHandler);

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
        std::lock_guard lock(eventQueueMutex);
        eventQueue.push(std::pair<std::promise<bool>, std::unique_ptr<Event>>(std::move(promise), std::move(event)));
#endif

        return future;
    }
}
