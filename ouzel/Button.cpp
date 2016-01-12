// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <functional>
#include "Button.h"
#include "Engine.h"
#include "Sprite.h"
#include "EventHander.h"
#include "EventDispatcher.h"
#include "Layer.h"
#include "Utils.h"

namespace ouzel
{
    Button::Button()
    {
        
    }
    
    Button::~Button()
    {
        Engine::getInstance()->getEventDispatcher()->removeEventHandler(_eventHandler);
    }
    
    bool Button::init(std::string const& normal, std::string const& selected, std::string const& pressed, std::string const& disabled, std::function<void(VoidPtr)> const& callback)
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
            _boundingBox = _normalSprite->getBoundingBox();
            addChild(_normalSprite);
        }
        
        _selectedSprite = std::make_shared<Sprite>();
        if (_selectedSprite->initFromFile(selected))
        {
            addChild(_selectedSprite);
        }
        
        _pressedSprite = std::make_shared<Sprite>();
        if (_pressedSprite->initFromFile(pressed))
        {
            addChild(_pressedSprite);
        }
        
        _disabledSprite = std::make_shared<Sprite>();
        if (_disabledSprite->initFromFile(disabled))
        {
            addChild(_disabledSprite);
        }
        
        _callback = callback;
        
        updateSprite();
        
        return true;
    }
    
    void Button::setEnabled(bool enabled)
    {
        Widget::setEnabled(enabled);
        
        _selected = false;
        _pointerOver = false;
        _pressed = false;
        
        updateSprite();
    }
    
    bool Button::handleMouseDown(const MouseEvent& event, VoidPtr const& sender)
    {
        if (!_enabled) return true;
            
        if (_pointerOver)
        {
            _pressed = true;
        }
        updateSprite();
        
        return true;
    }
    
    bool Button::handleMouseUp(const MouseEvent& event, VoidPtr const& sender)
    {
        if (!_enabled) return true;
        
        if (_pointerOver && _pressed)
        {
            if (_callback)
            {
                _callback(shared_from_this());
            }
        }
        
        _pressed = false;
        updateSprite();
        
        return true;
    }
    
    bool Button::handleMouseMove(const MouseEvent& event, VoidPtr const& sender)
    {
        if (!_enabled) return true;
        
        if (LayerPtr layer = _layer.lock())
        {
            Vector2 worldLocation = layer->screenToWorldLocation(event.position);
            checkPointer(worldLocation);
            updateSprite();
        }
        
        return true;
    }
    
    bool Button::handleTouchBegin(const TouchEvent& event, VoidPtr const& sender)
    {
        if (!_enabled) return true;
        
        if (LayerPtr layer = _layer.lock())
        {
            Vector2 worldLocation = layer->screenToWorldLocation(event.position);
            checkPointer(worldLocation);
            
            if (_pointerOver)
            {
                _pressed = true;
            }
            
            updateSprite();
        }
        
        return true;
    }
    
    bool Button::handleTouchMove(const TouchEvent& event, VoidPtr const& sender)
    {
        if (!_enabled) return true;
        
        if (LayerPtr layer = _layer.lock())
        {
            Vector2 worldLocation = layer->screenToWorldLocation(event.position);
            checkPointer(worldLocation);
            updateSprite();
        }
        
        return true;
    }
    
    bool Button::handleTouchEnd(const TouchEvent& event, VoidPtr const& sender)
    {
        if (!_enabled) return true;
        
        if (_pointerOver && _pressed)
        {
            if (_callback)
            {
                _callback(shared_from_this());
            }
        }
        
        _pressed = false;
        updateSprite();
        
        return true;
    }
    
    bool Button::handleGamepadButtonChange(const GamepadEvent& event, VoidPtr const& sender)
    {
        return true;
    }
    
    void Button::checkPointer(Vector2 const& worldLocation)
    {
        if (pointOn(worldLocation))
        {
            _pointerOver = true;
        }
        else
        {
            _pointerOver = false;
        }
    }
    
    void Button::updateSprite()
    {
        if (_normalSprite) _normalSprite->setVisible(false);
        if (_selectedSprite) _selectedSprite->setVisible(false);
        if (_pressedSprite) _pressedSprite->setVisible(false);
        if (_disabledSprite) _disabledSprite->setVisible(false);
        
        if (_enabled)
        {
            if (_pressed && _pointerOver && _pressedSprite)
            {
                _pressedSprite->setVisible(true);
            }
            else if (_selected && _selectedSprite)
            {
                _selectedSprite->setVisible(true);
            }
            else if (_normalSprite)
            {
                _normalSprite->setVisible(true);
            }
        }
        else if (_disabledSprite)
        {
            _disabledSprite->setVisible(true);
        }
    }
}
