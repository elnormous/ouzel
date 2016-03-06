// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Color.h"

namespace ouzel
{
namespace video
{
    Color::Color():
        r(0), g(0), b(0), a(0)
    {
        
    }
    
    Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a):
        r(r), g(g), b(b), a(a)
    {
        
    }
    
    float Color::getR() const
    {
        return r / 255.0f;
    }
    
    float Color::getG() const
    {
        return g / 255.0f;
    }
    
    float Color::getB() const
    {
        return b / 255.0f;
    }
    
    float Color::getA() const
    {
        return a / 255.0f;
    }
} // namespace video
} // namespace ouzel
