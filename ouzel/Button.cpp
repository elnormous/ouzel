// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <functional>
#include "Button.h"
#include "Engine.h"
#include "Sprite.h"
#include "EventHander.h"
#include "EventDispatcher.h"

namespace ouzel
{
    Button::Button()
    {
        
    }
    
    Button::~Button()
    {
        Engine::getInstance()->getEventDispatcher()->removeEventHandler(_eventHandler);
    }
    
    bool Button::init(std::string const& normal, std::string const& selected, std::string const& pressed)
    {
        _eventHandler = std::make_shared<EventHandler>();
        
        _eventHandler->mouseDownHandler = std::bind(&Button::handleMouseDown, this, std::placeholders::_1, std::placeholders::_2);
        _eventHandler->mouseUpHandler = std::bind(&Button::handleMouseUp, this, std::placeholders::_1, std::placeholders::_2);
        _eventHandler->mouseMoveHandler = std::bind(&Button::handleMouseMove, this, std::placeholders::_1, std::placeholders::_2);
        _eventHandler->touchBeginHandler = std::bind(&Button::handleTouchBegin, this, std::placeholders::_1, std::placeholders::_2);
        _eventHandler->touchMoveHandler = std::bind(&Button::handleTouchMove, this, std::placeholders::_1, std::placeholders::_2);
        _eventHandler->touchEndHandler = std::bind(&Button::handleTouchEnd, this, std::placeholders::_1, std::placeholders::_2);
        _eventHandler->gamepadButtonChangeHandler = std::bind(&Button::handleGamepadButtonChange, this, std::placeholders::_1, std::placeholders::_2);
        
        Engine::getInstance()->getEventDispatcher()->addEventHandler(_eventHandler);
        
        _normalSprite = std::make_shared<Sprite>();
        if (_normalSprite->initFromFile(normal))
        {
            addChild(_normalSprite);
        }
        
        _selectedSprite = std::make_shared<Sprite>();
        if (_selectedSprite->initFromFile(selected))
        {
            _selectedSprite->setVisible(false);
            addChild(_selectedSprite);
        }
        
        _pressedSprite = std::make_shared<Sprite>();
        if (_pressedSprite->initFromFile(pressed))
        {
            _pressedSprite->setVisible(false);
            addChild(_pressedSprite);
        }
        
        return true;
    }
    
    bool Button::handleMouseDown(const MouseEvent& event, std::shared_ptr<void> const& sender)
    {
        return true;
    }
    
    bool Button::handleMouseUp(const MouseEvent& event, std::shared_ptr<void> const& sender)
    {
        return true;
    }
    
    bool Button::handleMouseMove(const MouseEvent& event, std::shared_ptr<void> const& sender)
    {
        return true;
    }
    
    bool Button::handleTouchBegin(const TouchEvent& event, std::shared_ptr<void> const& sender)
    {
        return true;
    }
    
    bool Button::handleTouchMove(const TouchEvent& event, std::shared_ptr<void> const& sender)
    {
        return true;
    }
    
    bool Button::handleTouchEnd(const TouchEvent& event, std::shared_ptr<void> const& sender)
    {
        return true;
    }
    
    bool Button::handleGamepadButtonChange(const GamepadEvent& event, std::shared_ptr<void> const& sender)
    {
        return true;
    }
}
