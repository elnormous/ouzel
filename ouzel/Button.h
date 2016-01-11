// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Widget.h"

namespace ouzel
{
    class Sprite;
    
    class Button: public Widget
    {
    public:
        Button();
        virtual ~Button();
        
        virtual bool init(std::string const& normal, std::string const& selected, std::string const& pressed);
        
    protected:
        std::shared_ptr<Sprite> _normalSprite;
        std::shared_ptr<Sprite> _selectedSprite;
        std::shared_ptr<Sprite> _pressedSprite;
    };
}
