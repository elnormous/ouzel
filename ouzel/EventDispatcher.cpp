// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
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
        std::vector<SharedPtr<EventHandler>>::iterator i = std::find(_eventHandlers.begin(), _eventHandlers.end(), eventHandler);
        
        if (i == _eventHandlers.end())
        {
            _eventHandlers.push_back(eventHandler);
            
            std::sort(_eventHandlers.begin(), _eventHandlers.end(), [](EventHandler* a, EventHandler* b) {
                return a->_priority < b->_priority;
            });
        }
    }
    
    void EventDispatcher::removeEventHandler(EventHandler* eventHandler)
    {
        std::vector<SharedPtr<EventHandler>>::iterator i = std::find(_eventHandlers.begin(), _eventHandlers.end(), eventHandler);
        
        if (i != _eventHandlers.end())
        {
            _eventHandlers.erase(i);
        }
    }
    
    void EventDispatcher::dispatchKeyDownEvent(const KeyboardEvent& event, ReferenceCounted* sender)
    {
        for (SharedPtr<EventHandler> eventHandler : _eventHandlers)
        {
            if (eventHandler->keyDownHandler)
            {
                eventHandler->keyDownHandler(event, sender);
                break;
            }
        }
    }
    
    void EventDispatcher::dispatchKeyUpEvent(const KeyboardEvent& event, ReferenceCounted* sender)
    {
        for (SharedPtr<EventHandler> eventHandler : _eventHandlers)
        {
            if (eventHandler->keyUpHandler)
            {
                eventHandler->keyUpHandler(event, sender);
                break;
            }
        }
    }
    
    void EventDispatcher::dispatchMouseDownEvent(const MouseEvent& event, ReferenceCounted* sender)
    {
        for (SharedPtr<EventHandler> eventHandler : _eventHandlers)
        {
            if (eventHandler->mouseDownHandler)
            {
                eventHandler->mouseDownHandler(event, sender);
                break;
            }
        }
    }
    
    void EventDispatcher::dispatchMouseUpEvent(const MouseEvent& event, ReferenceCounted* sender)
    {
        for (SharedPtr<EventHandler> eventHandler : _eventHandlers)
        {
            if (eventHandler->mouseDownHandler)
            {
                eventHandler->mouseDownHandler(event, sender);
                break;
            }
        }
    }
    
    void EventDispatcher::dispatchMouseScrollEvent(const MouseEvent& event, ReferenceCounted* sender)
    {
        for (SharedPtr<EventHandler> eventHandler : _eventHandlers)
        {
            if (eventHandler->mouseScrollHandler)
            {
                eventHandler->mouseScrollHandler(event, sender);
                break;
            }
        }
    }
    
    void EventDispatcher::dispatchMouseMoveEvent(const MouseEvent& event, ReferenceCounted* sender)
    {
        for (SharedPtr<EventHandler> eventHandler : _eventHandlers)
        {
            if (eventHandler->mouseMoveHandler)
            {
                eventHandler->mouseMoveHandler(event, sender);
                break;
            }
        }
    }
    
    void EventDispatcher::dispatchMouseDragEvent(const MouseEvent& event, ReferenceCounted* sender)
    {
        for (SharedPtr<EventHandler> eventHandler : _eventHandlers)
        {
            if (eventHandler->mouseDragHandler)
            {
                eventHandler->mouseDragHandler(event, sender);
                break;
            }
        }
    }
    
    void EventDispatcher::dispatchTouchBeginEvent(const TouchEvent& event, ReferenceCounted* sender)
    {
        for (SharedPtr<EventHandler> eventHandler : _eventHandlers)
        {
            if (eventHandler->touchBeginHandler)
            {
                eventHandler->touchBeginHandler(event, sender);
                break;
            }
        }
    }
    
    void EventDispatcher::dispatchTouchMoveEvent(const TouchEvent& event, ReferenceCounted* sender)
    {
        for (SharedPtr<EventHandler> eventHandler : _eventHandlers)
        {
            if (eventHandler->touchMoveHandler)
            {
                eventHandler->touchMoveHandler(event, sender);
                break;
            }
        }
    }
    
    void EventDispatcher::dispatchTouchEndEvent(const TouchEvent& event, ReferenceCounted* sender)
    {
        for (SharedPtr<EventHandler> eventHandler : _eventHandlers)
        {
            if (eventHandler->touchEndHandler)
            {
                eventHandler->touchEndHandler(event, sender);
                break;
            }
        }
    }
    
    void EventDispatcher::dispatchTouchCancelEvent(const TouchEvent& event, ReferenceCounted* sender)
    {
        for (SharedPtr<EventHandler> eventHandler : _eventHandlers)
        {
            if (eventHandler->keyDownHandler)
            {
                eventHandler->touchCancelHandler(event, sender);
                break;
            }
        }
    }
    
    void EventDispatcher::dispatchScreenSizeEvent(const ScreenSizeEvent& event, ReferenceCounted* sender)
    {
        for (SharedPtr<EventHandler> eventHandler : _eventHandlers)
        {
            if (eventHandler->screenSizeHandler)
            {
                eventHandler->screenSizeHandler(event, sender);
                break;
            }
        }
    }
}