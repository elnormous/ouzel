#ifndef FLUXION_EVENTS_EVENTDISPATCHER_HPP
#define FLUXION_EVENTS_EVENTDISPATCHER_HPP

#include <cstdint>
#include <future>
#include <memory>
#include <queue>
#include <set>
#include <vector>
#include "Event.hpp"

namespace fluxion
{
    class EventHandler;

    class EventDispatcher final
    {
    public:
        EventDispatcher() = default;
        ~EventDispatcher();

        EventDispatcher(const EventDispatcher&) = delete;
        EventDispatcher& operator=(const EventDispatcher&) = delete;
        EventDispatcher(EventDispatcher&&) = delete;
        EventDispatcher& operator=(EventDispatcher&&) = delete;

        void addEventHandler(EventHandler& eventHandler);
        void removeEventHandler(EventHandler& eventHandler);

        // dispatches the event immediately
        bool dispatchEvent(std::unique_ptr<Event> event);

        // posts the event for dispatching on the game thread
        std::future<bool> postEvent(std::unique_ptr<Event> event);

        // dispatches all queued events on the game thread
        void dispatchEvents();

    private:
        std::vector<EventHandler*> eventHandlers;
        std::set<EventHandler*> eventHandlerAddSet;
        std::set<EventHandler*> eventHandlerDeleteSet;

        std::mutex eventQueueMutex;
        std::queue<std::pair<std::promise<bool>, std::unique_ptr<Event>>> eventQueue;
    };
}

#endif // FLUXION_EVENTS_EVENTDISPATCHER_HPP
