// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Size2.h"

namespace ouzel
{
    Size2::Size2():
        width(0.0f), height(0.0f)
    {
        
    }
    
    Size2::Size2(float width, float height):
        width(width), height(height)
    {
        
    }
    
    Size2& Size2::operator=(const Vector2& point)
    {
        width = point.x;
        height = point.y;
        
        return *this;
    }
    
    Size2 Size2::operator+(const Size2& right) const
    {
        return Size2(width + right.width, height + right.height);
    }
    
    Size2 Size2::operator-(const Size2& right) const
    {
        return Size2(width - right.width, height - right.height);
    }
    
    Size2 Size2::operator*(float a) const
    {
        return Size2(width * a, height * a);
    }
    
    Size2 Size2::operator/(float a) const
    {
        return Size2(width / a, height / a);
    }
    
    bool Size2::isZero() const
    {
        return width == 0.0f && height == 0.0f;
    }
}
