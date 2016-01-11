// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Button.h"
#include "Sprite.h"

namespace ouzel
{
    Button::Button()
    {
        
    }
    
    Button::~Button()
    {
        
    }
    
    bool Button::init(std::string const& normal, std::string const& selected, std::string const& pressed)
    {
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
}
