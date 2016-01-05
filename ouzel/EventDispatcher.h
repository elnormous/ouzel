// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "AutoPtr.h"
#include "Noncopyable.h"
#include "ReferenceCounted.h"
#include "Event.h"
#include "EventHander.h"

namespace ouzel
{
    class Engine;
    class EventHandler;
    
    class EventDispatcher: public Noncopyable, public ReferenceCounted
    {
        friend Engine;
    public:
        static EventDispatcher* getInstance();
        
        virtual ~EventDispatcher();
        
        void addEventHandler(EventHandler* eventHandler);
        void removeEventHandler(EventHandler* eventHandler);
        
        void dispatchEvent(const Event& event);
        
    protected:
        EventDispatcher();
        
        std::vector<AutoPtr<EventHandler>> _eventHandlers;
    };
}