// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Input.h"
#include "Engine.h"
#include "EventDispatcher.h"
#include "Utils.h"

namespace ouzel
{
    Input::Input()
    {
        memset(_keyboardKeyStates, 0, sizeof(_keyboardKeyStates));
        memset(_mouseButtonStates, 0, sizeof(_mouseButtonStates));
    }
    
    Input::~Input()
    {
        
    }
    
    void Input::update()
    {
        
    }
    
    void Input::setCursorVisible(bool visible)
    {
        OUZEL_UNUSED(visible);
    }
    
    bool Input::isCursorVisible() const
    {
        return false;
    }
    
    void Input::startGamepadDiscovery()
    {
        
    }
    
    void Input::stopGamepadDiscovery()
    {
        
    }
    
    void Input::keyDown(KeyboardKey key, uint32_t modifiers)
    {
        if (!_keyboardKeyStates[static_cast<uint32_t>(key)])
        {
            _keyboardKeyStates[static_cast<uint32_t>(key)] = true;
            
            KeyboardEvent event;
            event.type = Event::Type::KEY_DOWN;
            event.key = key;
            event.modifiers = modifiers;
            
            Engine::getInstance()->getEventDispatcher()->dispatchKeyDownEvent(event, shared_from_this());
        }
        else
        {
            // TODO: implement key repeat event
        }
    }
    
    void Input::keyUp(KeyboardKey key, uint32_t modifiers)
    {
        _keyboardKeyStates[static_cast<uint32_t>(key)] = false;
        
        KeyboardEvent event;
        event.type = Event::Type::KEY_UP;
        event.key = key;
        event.modifiers = modifiers;
        
        Engine::getInstance()->getEventDispatcher()->dispatchKeyUpEvent(event, shared_from_this());
    }
    
    void Input::mouseDown(MouseButton button, const Vector2& position, uint32_t modifiers)
    {
        _mouseButtonStates[static_cast<uint32_t>(button)] = true;
        
        MouseEvent event;
        event.type = Event::Type::MOUSE_DOWN;
        event.button = button;
        event.position = position;
        event.modifiers = modifiers;
        
        Engine::getInstance()->getEventDispatcher()->dispatchMouseDownEvent(event, shared_from_this());
    }
    
    void Input::mouseUp(MouseButton button, const Vector2& position, uint32_t modifiers)
    {
        _mouseButtonStates[static_cast<uint32_t>(button)] = false;
        
        MouseEvent event;
        event.type = Event::Type::MOUSE_UP;
        event.button = button;
        event.position = position;
        event.modifiers = modifiers;
        
        Engine::getInstance()->getEventDispatcher()->dispatchMouseUpEvent(event, shared_from_this());
    }
    
    void Input::mouseMove(const Vector2& position, uint32_t modifiers)
    {
        MouseEvent event;
        event.type = Event::Type::MOUSE_MOVE;
        event.position = position;
        event.modifiers = modifiers;
        
        Engine::getInstance()->getEventDispatcher()->dispatchMouseMoveEvent(event, shared_from_this());
    }
    
    void Input::mouseScroll(const Vector2& scroll, const Vector2& position, uint32_t modifiers)
    {
        MouseEvent event;
        event.type = Event::Type::MOUSE_SCROLL;
        event.position = position;
        event.scroll = scroll;
        event.modifiers = modifiers;
        
        Engine::getInstance()->getEventDispatcher()->dispatchMouseScrollEvent(event, shared_from_this());
    }
    
    void Input::touchBegin(uint64_t touchId, const Vector2 position)
    {
        TouchEvent event;
        event.type = Event::Type::TOUCH_BEGIN;
        event.touchId = touchId;
        event.position = position;
        
        Engine::getInstance()->getEventDispatcher()->dispatchTouchBeginEvent(event, shared_from_this());
    }
    
    void Input::touchEnd(uint64_t touchId, const Vector2 position)
    {
        TouchEvent event;
        event.type = Event::Type::TOUCH_END;
        event.touchId = touchId;
        event.position = position;
        
        Engine::getInstance()->getEventDispatcher()->dispatchTouchEndEvent(event, shared_from_this());
    }
    
    void Input::touchMove(uint64_t touchId, const Vector2 position)
    {
        TouchEvent event;
        event.type = Event::Type::TOUCH_MOVE;
        event.touchId = touchId;
        event.position = position;
        
        Engine::getInstance()->getEventDispatcher()->dispatchTouchMoveEvent(event, shared_from_this());
    }
    
    void Input::touchCancel(uint64_t touchId, const Vector2 position)
    {
        TouchEvent event;
        event.type = Event::Type::TOUCH_CANCEL;
        event.touchId = touchId;
        event.position = position;
        
        Engine::getInstance()->getEventDispatcher()->dispatchTouchCancelEvent(event, shared_from_this());
    }
}
