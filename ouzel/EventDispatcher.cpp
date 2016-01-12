// Copyright (C) 2015 Elviss Strazdins
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
    
    void EventDispatcher::addEventHandler(EventHandlerPtr const& eventHandler)
    {
        std::vector<EventHandlerPtr>::iterator i = std::find_if(_eventHandlers.begin(), _eventHandlers.end(), [eventHandler](EventHandlerPtr const& p) {
            return p.get() == eventHandler.get();
        });
        
        if (i == _eventHandlers.end())
        {
            _eventHandlers.push_back(eventHandler);
            
            std::sort(_eventHandlers.begin(), _eventHandlers.end(), [](EventHandlerPtr const& a, EventHandlerPtr const& b) {
                return a->_priority < b->_priority;
            });
        }
    }
    
    void EventDispatcher::removeEventHandler(EventHandlerPtr const& eventHandler)
    {
        if (_locked)
        {
            _eventHandlerDeleteList.insert(eventHandler);
        }
        else
        {
            std::vector<EventHandlerPtr>::iterator i = std::find_if(_eventHandlers.begin(), _eventHandlers.end(), [eventHandler](EventHandlerPtr const& p) {
                return p.get() == eventHandler.get();
            });
            
            if (i != _eventHandlers.end())
            {
                _eventHandlers.erase(i);
            }
        }
    }
    
    void EventDispatcher::dispatchKeyDownEvent(const KeyboardEvent& event, VoidPtr const& sender)
    {
        lock();
        
        for (EventHandlerPtr const& eventHandler : _eventHandlers)
        {
            if (eventHandler->keyDownHandler)
            {
                if (!eventHandler->keyDownHandler(event, sender))
                {
                    break;
                }
            }
        }
        
        unlock();
    }
    
    void EventDispatcher::dispatchKeyUpEvent(const KeyboardEvent& event, VoidPtr const& sender)
    {
        lock();
        
        for (EventHandlerPtr const& eventHandler : _eventHandlers)
        {
            if (eventHandler->keyUpHandler)
            {
                if (!eventHandler->keyUpHandler(event, sender))
                {
                    break;
                }
            }
        }
        
        unlock();
    }
    
    void EventDispatcher::dispatchMouseDownEvent(const MouseEvent& event, VoidPtr const& sender)
    {
        lock();
        
        for (EventHandlerPtr const& eventHandler : _eventHandlers)
        {
            if (eventHandler->mouseDownHandler)
            {
                if (!eventHandler->mouseDownHandler(event, sender))
                {
                    break;
                }
            }
        }
        
        unlock();
    }
    
    void EventDispatcher::dispatchMouseUpEvent(const MouseEvent& event, VoidPtr const& sender)
    {
        lock();
        
        for (EventHandlerPtr const& eventHandler : _eventHandlers)
        {
            if (eventHandler->mouseUpHandler)
            {
                if (!eventHandler->mouseUpHandler(event, sender))
                {
                    break;
                }
            }
        }
        
        unlock();
    }
    
    void EventDispatcher::dispatchMouseScrollEvent(const MouseEvent& event, VoidPtr const& sender)
    {
        lock();
        
        for (EventHandlerPtr const& eventHandler : _eventHandlers)
        {
            if (eventHandler->mouseScrollHandler)
            {
                if (!eventHandler->mouseScrollHandler(event, sender))
                {
                    break;
                }
            }
        }
        
        unlock();
    }
    
    void EventDispatcher::dispatchMouseMoveEvent(const MouseEvent& event, VoidPtr const& sender)
    {
        lock();
        
        for (EventHandlerPtr const& eventHandler : _eventHandlers)
        {
            if (eventHandler->mouseMoveHandler)
            {
                if (!eventHandler->mouseMoveHandler(event, sender))
                {
                    break;
                }
            }
        }
        
        unlock();
    }
    
    void EventDispatcher::dispatchTouchBeginEvent(const TouchEvent& event, VoidPtr const& sender)
    {
        lock();
        
        for (EventHandlerPtr const& eventHandler : _eventHandlers)
        {
            if (eventHandler->touchBeginHandler)
            {
                if (!eventHandler->touchBeginHandler(event, sender))
                {
                    break;
                }
            }
        }
        
        unlock();
    }
    
    void EventDispatcher::dispatchTouchMoveEvent(const TouchEvent& event, VoidPtr const& sender)
    {
        lock();
        
        for (EventHandlerPtr const& eventHandler : _eventHandlers)
        {
            if (eventHandler->touchMoveHandler)
            {
                if (!eventHandler->touchMoveHandler(event, sender))
                {
                    break;
                }
            }
        }
        
        unlock();
    }
    
    void EventDispatcher::dispatchTouchEndEvent(const TouchEvent& event, VoidPtr const& sender)
    {
        lock();
        
        for (EventHandlerPtr const& eventHandler : _eventHandlers)
        {
            if (eventHandler->touchEndHandler)
            {
                if (!eventHandler->touchEndHandler(event, sender))
                {
                    break;
                }
            }
        }
        
        unlock();
    }
    
    void EventDispatcher::dispatchTouchCancelEvent(const TouchEvent& event, VoidPtr const& sender)
    {
        lock();
        
        for (EventHandlerPtr const& eventHandler : _eventHandlers)
        {
            if (eventHandler->keyDownHandler)
            {
                if (!eventHandler->touchCancelHandler(event, sender))
                {
                    break;
                }
            }
        }
        
        unlock();
    }
    
    void EventDispatcher::dispatchGamepadConnectEvent(const GamepadEvent& event, VoidPtr const& sender)
    {
        lock();
        
        for (EventHandlerPtr const& eventHandler : _eventHandlers)
        {
            if (eventHandler->gamepadConnectHandler)
            {
                if (!eventHandler->gamepadConnectHandler(event, sender))
                {
                    break;
                }
            }
        }
        
        unlock();
    }
    
    void EventDispatcher::dispatchGamepadDisconnectEvent(const GamepadEvent& event, VoidPtr const& sender)
    {
        lock();
        
        for (EventHandlerPtr const& eventHandler : _eventHandlers)
        {
            if (eventHandler->gamepadDisconnectHandler)
            {
                if (!eventHandler->gamepadDisconnectHandler(event, sender))
                {
                    break;
                }
            }
        }
        
        unlock();
    }
    
    void EventDispatcher::dispatchGamepadButtonChangeEvent(const GamepadEvent& event, VoidPtr const& sender)
    {
        lock();
        
        for (EventHandlerPtr const& eventHandler : _eventHandlers)
        {
            if (eventHandler->gamepadButtonChangeHandler)
            {
                if (!eventHandler->gamepadButtonChangeHandler(event, sender))
                {
                    break;
                }
            }
        }
        
        unlock();
    }
    
    void EventDispatcher::dispatchWindowSizeChangeEvent(const WindowEvent& event, VoidPtr const& sender)
    {
        lock();
        
        for (EventHandlerPtr const& eventHandler : _eventHandlers)
        {
            if (eventHandler->windowSizeChangeHandler)
            {
                if (!eventHandler->windowSizeChangeHandler(event, sender))
                {
                    break;
                }
            }
        }
        
        unlock();
    }
    
    void EventDispatcher::dispatchWindowTitleChangeEvent(const WindowEvent& event, VoidPtr const& sender)
    {
        lock();
        
        for (EventHandlerPtr const& eventHandler : _eventHandlers)
        {
            if (eventHandler->windowTitleChangeHandler)
            {
                if (!eventHandler->windowTitleChangeHandler(event, sender))
                {
                    break;
                }
            }
        }
        
        unlock();
    }
    
    void EventDispatcher::lock()
    {
        _locked = true;
    }
    
    void EventDispatcher::unlock()
    {
        _locked = false;
        
        for (EventHandlerPtr const& eventHandler : _eventHandlerDeleteList)
        {
            removeEventHandler(eventHandler);
        }
        
        _eventHandlerDeleteList.clear();
    }
}
