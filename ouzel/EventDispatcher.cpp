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
    
    void EventDispatcher::addEventHandler(std::shared_ptr<EventHandler> const& eventHandler)
    {
        std::vector<std::shared_ptr<EventHandler>>::iterator i = std::find_if(_eventHandlers.begin(), _eventHandlers.end(), [eventHandler](std::shared_ptr<EventHandler> const& p) {
            return p.get() == eventHandler.get();
        });
        
        if (i == _eventHandlers.end())
        {
            _eventHandlers.push_back(eventHandler);
            
            std::sort(_eventHandlers.begin(), _eventHandlers.end(), [](std::shared_ptr<EventHandler> const& a, std::shared_ptr<EventHandler> const& b) {
                return a->_priority < b->_priority;
            });
        }
    }
    
    void EventDispatcher::removeEventHandler(std::shared_ptr<EventHandler> const& eventHandler)
    {
        if (_locked)
        {
            _eventHandlerDeleteList.insert(eventHandler);
        }
        else
        {
            std::vector<std::shared_ptr<EventHandler>>::iterator i = std::find_if(_eventHandlers.begin(), _eventHandlers.end(), [eventHandler](std::shared_ptr<EventHandler> const& p) {
                return p.get() == eventHandler.get();
            });
            
            if (i != _eventHandlers.end())
            {
                _eventHandlers.erase(i);
            }
        }
    }
    
    void EventDispatcher::dispatchKeyDownEvent(const KeyboardEvent& event, std::shared_ptr<void> const& sender)
    {
        lock();
        
        for (std::shared_ptr<EventHandler> const& eventHandler : _eventHandlers)
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
    
    void EventDispatcher::dispatchKeyUpEvent(const KeyboardEvent& event, std::shared_ptr<void> const& sender)
    {
        lock();
        
        for (std::shared_ptr<EventHandler> const& eventHandler : _eventHandlers)
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
    
    void EventDispatcher::dispatchMouseDownEvent(const MouseEvent& event, std::shared_ptr<void> const& sender)
    {
        lock();
        
        for (std::shared_ptr<EventHandler> const& eventHandler : _eventHandlers)
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
    
    void EventDispatcher::dispatchMouseUpEvent(const MouseEvent& event, std::shared_ptr<void> const& sender)
    {
        lock();
        
        for (std::shared_ptr<EventHandler> const& eventHandler : _eventHandlers)
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
    
    void EventDispatcher::dispatchMouseScrollEvent(const MouseEvent& event, std::shared_ptr<void> const& sender)
    {
        lock();
        
        for (std::shared_ptr<EventHandler> const& eventHandler : _eventHandlers)
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
    
    void EventDispatcher::dispatchMouseMoveEvent(const MouseEvent& event, std::shared_ptr<void> const& sender)
    {
        lock();
        
        for (std::shared_ptr<EventHandler> const& eventHandler : _eventHandlers)
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
    
    void EventDispatcher::dispatchTouchBeginEvent(const TouchEvent& event, std::shared_ptr<void> const& sender)
    {
        lock();
        
        for (std::shared_ptr<EventHandler> const& eventHandler : _eventHandlers)
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
    
    void EventDispatcher::dispatchTouchMoveEvent(const TouchEvent& event, std::shared_ptr<void> const& sender)
    {
        lock();
        
        for (std::shared_ptr<EventHandler> const& eventHandler : _eventHandlers)
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
    
    void EventDispatcher::dispatchTouchEndEvent(const TouchEvent& event, std::shared_ptr<void> const& sender)
    {
        lock();
        
        for (std::shared_ptr<EventHandler> const& eventHandler : _eventHandlers)
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
    
    void EventDispatcher::dispatchTouchCancelEvent(const TouchEvent& event, std::shared_ptr<void> const& sender)
    {
        lock();
        
        for (std::shared_ptr<EventHandler> const& eventHandler : _eventHandlers)
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
    
    void EventDispatcher::dispatchGamepadConnectEvent(const GamepadEvent& event, std::shared_ptr<void> const& sender)
    {
        lock();
        
        for (std::shared_ptr<EventHandler> const& eventHandler : _eventHandlers)
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
    
    void EventDispatcher::dispatchGamepadDisconnectEvent(const GamepadEvent& event, std::shared_ptr<void> const& sender)
    {
        lock();
        
        for (std::shared_ptr<EventHandler> const& eventHandler : _eventHandlers)
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
    
    void EventDispatcher::dispatchGamepadButtonChangeEvent(const GamepadEvent& event, std::shared_ptr<void> const& sender)
    {
        lock();
        
        for (std::shared_ptr<EventHandler> const& eventHandler : _eventHandlers)
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
    
    void EventDispatcher::dispatchScreenSizeEvent(const ScreenSizeEvent& event, std::shared_ptr<void> const& sender)
    {
        lock();
        
        for (std::shared_ptr<EventHandler> const& eventHandler : _eventHandlers)
        {
            if (eventHandler->screenSizeHandler)
            {
                if (!eventHandler->screenSizeHandler(event, sender))
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
        
        for (std::shared_ptr<EventHandler> const& eventHandler : _eventHandlerDeleteList)
        {
            removeEventHandler(eventHandler);
        }
        
        _eventHandlerDeleteList.clear();
    }
}
