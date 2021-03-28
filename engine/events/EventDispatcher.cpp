// Ouzel by Elviss Strazdins

#include <algorithm>
#include "EventDispatcher.hpp"
#include "EventHandler.hpp"
#include "../utils/Utils.hpp"

namespace ouzel
{
    EventDispatcher::~EventDispatcher()
    {
        for (auto eventHandler : eventHandlerAddSet)
            eventHandler->eventDispatcher = nullptr;

        for (auto eventHandler : eventHandlers)
            if (std::find(eventHandlerDeleteSet.begin(),
                          eventHandlerDeleteSet.end(),
                          eventHandler) == eventHandlerDeleteSet.end())
                eventHandler->eventDispatcher = nullptr;
    }

    void EventDispatcher::dispatchEvents()
    {
        for (const auto eventHandler : eventHandlerDeleteSet)
            if (const auto i = std::find(eventHandlers.begin(),
                                         eventHandlers.end(),
                                         eventHandler); i != eventHandlers.end())
                eventHandlers.erase(i);

        eventHandlerDeleteSet.clear();

        for (auto eventHandler : eventHandlerAddSet)
            if (std::find(eventHandlers.begin(),
                          eventHandlers.end(),
                          eventHandler) == eventHandlers.end())
            {
                const auto upperBound = std::upper_bound(eventHandlers.begin(), eventHandlers.end(), eventHandler,
                                                         [](const auto a, const auto b) noexcept {
                                                                return a->priority > b->priority;
                                                            });

                eventHandlers.insert(upperBound, eventHandler);
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

        for (const auto eventHandler : eventHandlers)
            if (std::find(eventHandlerDeleteSet.begin(),
                          eventHandlerDeleteSet.end(),
                          eventHandler) == eventHandlerDeleteSet.end())
                switch (event->type)
                {
                    case Event::Type::keyboardConnect:
                    case Event::Type::keyboardDisconnect:
                    case Event::Type::keyboardKeyPress:
                    case Event::Type::keyboardKeyRelease:
                        if (eventHandler->keyboardHandler &&
                            eventHandler->keyboardHandler(*static_cast<KeyboardEvent*>(event.get())))
                            return true;
                        break;
                    case Event::Type::mouseConnect:
                    case Event::Type::mouseDisconnect:
                    case Event::Type::mousePress:
                    case Event::Type::mouseRelease:
                    case Event::Type::mouseScroll:
                    case Event::Type::mouseMove:
                    case Event::Type::mouseCursorLockChange:
                        if (eventHandler->mouseHandler &&
                            eventHandler->mouseHandler(*static_cast<MouseEvent*>(event.get())))
                            return true;
                        break;
                    case Event::Type::touchpadConnect:
                    case Event::Type::touchpadDisconnect:
                    case Event::Type::touchBegin:
                    case Event::Type::touchMove:
                    case Event::Type::touchEnd:
                    case Event::Type::touchCancel:
                        if (eventHandler->touchHandler &&
                            eventHandler->touchHandler(*static_cast<TouchEvent*>(event.get())))
                            return true;
                        break;
                    case Event::Type::gamepadConnect:
                    case Event::Type::gamepadDisconnect:
                    case Event::Type::gamepadButtonChange:
                        if (eventHandler->gamepadHandler &&
                            eventHandler->gamepadHandler(*static_cast<GamepadEvent*>(event.get())))
                            return true;
                        break;
                    case Event::Type::windowSizeChange:
                    case Event::Type::windowTitleChange:
                    case Event::Type::fullscreenChange:
                    case Event::Type::screenChange:
                    case Event::Type::resolutionChange:
                        if (eventHandler->windowHandler &&
                            eventHandler->windowHandler(*static_cast<WindowEvent*>(event.get())))
                            return true;
                        break;
                    case Event::Type::engineStart:
                    case Event::Type::engineStop:
                    case Event::Type::engineResume:
                    case Event::Type::enginePause:
                    case Event::Type::orientationChange:
                    case Event::Type::lowMemory:
                    case Event::Type::openFile:
                        if (eventHandler->systemHandler &&
                            eventHandler->systemHandler(*static_cast<SystemEvent*>(event.get())))
                            return true;
                        break;
                    case Event::Type::actorEnter:
                    case Event::Type::actorLeave:
                    case Event::Type::actorPress:
                    case Event::Type::actorRelease:
                    case Event::Type::actorClick:
                    case Event::Type::actorDrag:
                    case Event::Type::widgetChange:
                        if (eventHandler->uiHandler &&
                            eventHandler->uiHandler(*static_cast<UIEvent*>(event.get())))
                            return true;
                        break;
                    case Event::Type::animationStart:
                    case Event::Type::animationReset:
                    case Event::Type::animationFinish:
                        if (eventHandler->animationHandler &&
                            eventHandler->animationHandler(*static_cast<AnimationEvent*>(event.get())))
                            return true;
                        break;
                    case Event::Type::soundStart:
                    case Event::Type::soundReset:
                    case Event::Type::soundFinish:
                        if (eventHandler->soundHandler &&
                            eventHandler->soundHandler(*static_cast<SoundEvent*>(event.get())))
                            return true;
                        break;
                    case Event::Type::update:
                        if (eventHandler->updateHandler &&
                            eventHandler->updateHandler(*static_cast<UpdateEvent*>(event.get())))
                            return true;
                        break;
                    case Event::Type::user:
                        if (eventHandler->userHandler &&
                            eventHandler->userHandler(*static_cast<UserEvent*>(event.get())))
                            return true;
                        break;
                    default:
                        return false; // custom event should not be sent
                }

        return false;
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

#ifdef __EMSCRIPTEN__
        promise.set_value(dispatchEvent(std::move(event)));
#else
        std::lock_guard lock(eventQueueMutex);
        eventQueue.push(std::pair<std::promise<bool>, std::unique_ptr<Event>>(std::move(promise), std::move(event)));
#endif

        return future;
    }
}
