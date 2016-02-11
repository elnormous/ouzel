// Copyright (C) 2016 Elviss Strazdins
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
    
    void EventDispatcher::update()
    {
        std::lock_guard<std::mutex> mutexLock(_mutex);
        
        lock();
        
        while (!_eventQueue.empty())
        {
            auto eventPair = _eventQueue.front();
            
            switch (eventPair.first->type)
            {
                case Event::Type::KEY_DOWN:
                case Event::Type::KEY_UP:
                case Event::Type::KEY_REPEAT:
                    dispatchKeyboardEvent(std::static_pointer_cast<KeyboardEvent>(eventPair.first), eventPair.second);
                    break;
                    
                case Event::Type::MOUSE_DOWN:
                case Event::Type::MOUSE_UP:
                case Event::Type::MOUSE_SCROLL:
                case Event::Type::MOUSE_MOVE:
                    dispatchMouseEvent(std::static_pointer_cast<MouseEvent>(eventPair.first), eventPair.second);
                    break;
                case Event::Type::TOUCH_BEGIN:
                case Event::Type::TOUCH_MOVE:
                case Event::Type::TOUCH_END:
                case Event::Type::TOUCH_CANCEL:
                    dispatchTouchEvent(std::static_pointer_cast<TouchEvent>(eventPair.first), eventPair.second);
                    break;
                case Event::Type::GAMEPAD_CONNECT:
                case Event::Type::GAMEPAD_DISCONNECT:
                case Event::Type::GAMEPAD_BUTTON_CHANGE:
                    dispatchGamepadEvent(std::static_pointer_cast<GamepadEvent>(eventPair.first), eventPair.second);
                    break;
                case Event::Type::WINDOW_SIZE_CHANGE:
                case Event::Type::WINDOW_TITLE_CHANGE:
                    dispatchWindowEvent(std::static_pointer_cast<WindowEvent>(eventPair.first), eventPair.second);
                    break;
            }
            
            _eventQueue.pop();
        }
        
        unlock();
    }
    
    void EventDispatcher::addEventHandler(const EventHandlerPtr& eventHandler)
    {
        if (_locked)
        {
            _eventHandlerAddList.insert(eventHandler);
        }
        else
        {
            std::vector<EventHandlerPtr>::iterator i = std::find(_eventHandlers.begin(), _eventHandlers.end(), eventHandler);
            
            if (i == _eventHandlers.end())
            {
                _eventHandlers.push_back(eventHandler);
                
                std::sort(_eventHandlers.begin(), _eventHandlers.end(), [](const EventHandlerPtr& a, const EventHandlerPtr& b) {
                    return a->_priority < b->_priority;
                });
            }
        }
    }
    
    void EventDispatcher::removeEventHandler(const EventHandlerPtr& eventHandler)
    {
        if (_locked)
        {
            eventHandler->_remove = true;
            _eventHandlerRemoveList.insert(eventHandler);
        }
        else
        {
            std::vector<EventHandlerPtr>::iterator i = std::find(_eventHandlers.begin(), _eventHandlers.end(), eventHandler);
            
            if (i != _eventHandlers.end())
            {
                _eventHandlers.erase(i);
            }
        }
    }
    
    void EventDispatcher::dispatchEvent(const EventPtr& event, const VoidPtr& sender)
    {
        std::lock_guard<std::mutex> mutexLock(_mutex);
        
        _eventQueue.push(std::make_pair(event, sender));
    }
    
    void EventDispatcher::dispatchKeyboardEvent(const KeyboardEventPtr& event, const VoidPtr& sender)
    {
        for (const EventHandlerPtr& eventHandler : _eventHandlers)
        {
            if (!eventHandler->_remove && eventHandler->keyboardHandler)
            {
                if (!eventHandler->keyboardHandler(event, sender))
                {
                    break;
                }
            }
        }
    }
    
    void EventDispatcher::dispatchMouseEvent(const MouseEventPtr& event, const VoidPtr& sender)
    {
        for (const EventHandlerPtr& eventHandler : _eventHandlers)
        {
            if (!eventHandler->_remove && eventHandler->mouseHandler)
            {
                if (!eventHandler->mouseHandler(event, sender))
                {
                    break;
                }
            }
        }
    }
    
    void EventDispatcher::dispatchTouchEvent(const TouchEventPtr& event, const VoidPtr& sender)
    {
        lock();
        
        for (const EventHandlerPtr& eventHandler : _eventHandlers)
        {
            if (!eventHandler->_remove && eventHandler->touchHandler)
            {
                if (!eventHandler->touchHandler(event, sender))
                {
                    break;
                }
            }
        }
        
        unlock();
    }
    
    void EventDispatcher::dispatchGamepadEvent(const GamepadEventPtr& event, const VoidPtr& sender)
    {
        for (const EventHandlerPtr& eventHandler : _eventHandlers)
        {
            if (!eventHandler->_remove && eventHandler->gamepadHandler)
            {
                if (!eventHandler->gamepadHandler(event, sender))
                {
                    break;
                }
            }
        }
    }
    
    void EventDispatcher::dispatchWindowEvent(const WindowEventPtr& event, const VoidPtr& sender)
    {
        for (const EventHandlerPtr& eventHandler : _eventHandlers)
        {
            if (!eventHandler->_remove && eventHandler->windowHandler)
            {
                if (!eventHandler->windowHandler(event, sender))
                {
                    break;
                }
            }
        }
    }
    
    void EventDispatcher::lock()
    {
        ++_locked;
    }
    
    void EventDispatcher::unlock()
    {
        if (--_locked == 0)
        {
            for (const EventHandlerPtr& eventHandler : _eventHandlerAddList)
            {
                addEventHandler(eventHandler);
            }
            
            for (const EventHandlerPtr& eventHandler : _eventHandlerRemoveList)
            {
                removeEventHandler(eventHandler);
            }
            
            _eventHandlerAddList.clear();
            _eventHandlerRemoveList.clear();
        }
    }
}
