// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <functional>
#include "Button.h"
#include "Engine.h"
#include "Sprite.h"
#include "EventHander.h"
#include "EventDispatcher.h"
#include "Layer.h"
#include "Camera.h"
#include "Label.h"
#include "Utils.h"

namespace ouzel
{
namespace gui
{
    std::shared_ptr<Button> Button::create(const std::string& normal, const std::string& selected, const std::string& pressed, const std::string& disabled,
                                           const std::string& label, const Color& labelColor, const std::string& font,
                                           const std::function<void(const VoidPtr&)>& callback)
    {
        std::shared_ptr<Button> result = std::make_shared<Button>();
        
        if (!result->init(normal, selected, pressed, disabled, label, labelColor, font, callback))
        {
            result.reset();
        }
        
        return result;
    }
    
    Button::Button()
    {
        
    }
    
    Button::~Button()
    {
        Engine::getInstance()->getEventDispatcher()->removeEventHandler(_eventHandler);
    }
    
    bool Button::init(const std::string& normal, const std::string& selected, const std::string& pressed, const std::string& disabled,
                      const std::string& label, const Color& labelColor, const std::string& font, const std::function<void(const VoidPtr&)>& callback)
    {
        _eventHandler = std::make_shared<EventHandler>();
        
        _eventHandler->mouseHandler = std::bind(&Button::handleMouse, this, std::placeholders::_1, std::placeholders::_2);
        _eventHandler->touchHandler = std::bind(&Button::handleTouch, this, std::placeholders::_1, std::placeholders::_2);
        _eventHandler->gamepadHandler = std::bind(&Button::handleGamepad, this, std::placeholders::_1, std::placeholders::_2);
        
        Engine::getInstance()->getEventDispatcher()->addEventHandler(_eventHandler);
        
        if (!normal.empty())
        {
            _normalSprite = std::make_shared<Sprite>();
            if (_normalSprite->initFromFile(normal, false))
            {
                _boundingBox = _normalSprite->getBoundingBox();
                addChild(_normalSprite);
            }
        }
        
        if (!selected.empty())
        {
            _selectedSprite = std::make_shared<Sprite>();
            if (_selectedSprite->initFromFile(selected, false))
            {
                _boundingBox.merge(_selectedSprite->getBoundingBox());
                addChild(_selectedSprite);
            }
        }
        
        if (!pressed.empty())
        {
            _pressedSprite = std::make_shared<Sprite>();
            if (_pressedSprite->initFromFile(pressed, false))
            {
                _boundingBox.merge(_pressedSprite->getBoundingBox());
                addChild(_pressedSprite);
            }
        }
        
        if (!disabled.empty())
        {
            _disabledSprite = std::make_shared<Sprite>();
            if (_disabledSprite->initFromFile(disabled, false))
            {
                _boundingBox.merge(_disabledSprite->getBoundingBox());
                addChild(_disabledSprite);
            }
        }
        
        if (!label.empty())
        {
            _label = Label::create(font, label);
            if (_label)
            {
                _boundingBox.merge(_label->getBoundingBox());
                _label->setColor(labelColor);
                addChild(_label);
            }
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
    
    bool Button::handleMouse(const MouseEventPtr& event, const VoidPtr& sender)
    {
        OUZEL_UNUSED(sender);
        
        if (!_enabled) return true;
        
        switch (event->type)
        {
            case Event::Type::MOUSE_DOWN:
            {
                if (_pointerOver)
                {
                    _pressed = true;
                    updateSprite();
                }
                break;
            }
            case Event::Type::MOUSE_UP:
            {
                if (_pointerOver && _pressed)
                {
                    _pressed = false;
                    updateSprite();
                    
                    if (_callback)
                    {
                        _callback(shared_from_this());
                    }
                }
                break;
            }
            case Event::Type::MOUSE_MOVE:
            {
                if (LayerPtr layer = _layer.lock())
                {
                    Vector2 worldLocation = layer->getCamera()->screenToWorldLocation(event->position);
                    checkPointer(worldLocation);
                    updateSprite();
                }
                break;
            }
            default:
                break;
        }
        
        return true;
    }
    
    bool Button::handleTouch(const TouchEventPtr& event, const VoidPtr& sender)
    {
        OUZEL_UNUSED(sender);
        
        if (!_enabled) return true;
        
        switch (event->type)
        {
            case Event::Type::TOUCH_BEGIN:
            {
                if (LayerPtr layer = _layer.lock())
                {
                    Vector2 worldLocation = layer->getCamera()->screenToWorldLocation(event->position);
                    checkPointer(worldLocation);
                    
                    if (_pointerOver)
                    {
                        _pressed = true;
                    }
                    
                    updateSprite();
                }
                break;
            }
            case Event::Type::TOUCH_MOVE:
            {
                if (LayerPtr layer = _layer.lock())
                {
                    Vector2 worldLocation = layer->getCamera()->screenToWorldLocation(event->position);
                    checkPointer(worldLocation);
                    updateSprite();
                }
                break;
            }
            case Event::Type::TOUCH_END:
            {
                if (_pointerOver && _pressed)
                {
                    _pressed = false;
                    updateSprite();
                    
                    if (_callback)
                    {
                        _callback(shared_from_this());
                    }
                }
                
                break;
            }
            default:
                break;
        }
        
        return true;
    }
    
    bool Button::handleGamepad(const GamepadEventPtr& event, const VoidPtr& sender)
    {
        OUZEL_UNUSED(event);
        OUZEL_UNUSED(sender);
        
        return true;
    }
    
    void Button::checkPointer(const Vector2& worldLocation)
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
} // namespace gui
} // namespace ouzel
