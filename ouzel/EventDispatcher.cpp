// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "EventDispatcher.h"

namespace ouzel
{
    static EventDispatcher* sharedEventDispatcher = nullptr;
    
    EventDispatcher* EventDispatcher::getInstance()
    {
        return sharedEventDispatcher;
    }
    
    EventDispatcher::EventDispatcher()
    {
        sharedEventDispatcher = this;
    }
    
    EventDispatcher::~EventDispatcher()
    {
        sharedEventDispatcher = nullptr;
    }
    
    void EventDispatcher::addEventHandler(EventHandler* eventHandler)
    {
        std::vector<AutoPtr<EventHandler>>::iterator i = std::find(_eventHandlers.begin(), _eventHandlers.end(), eventHandler);
        
        if (i == _eventHandlers.end())
        {
            _eventHandlers.push_back(eventHandler);
        }
    }
    
    void EventDispatcher::removeEventHandler(EventHandler* eventHandler)
    {
        std::vector<AutoPtr<EventHandler>>::iterator i = std::find(_eventHandlers.begin(), _eventHandlers.end(), eventHandler);
        
        if (i != _eventHandlers.end())
        {
            _eventHandlers.erase(i);
        }
    }
    
    void EventDispatcher::dispatchEvent(const Event& event)
    {
        for (AutoPtr<EventHandler> eventHandler : _eventHandlers)
        {
            if (!eventHandler->handleEvent(event))
            {
                break;
            }
        }
    }
}