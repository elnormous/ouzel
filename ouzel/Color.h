// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <stdint.h>

namespace ouzel
{
    namespace video
    {
        class Color
        {
        public:
            uint8_t r;
            uint8_t g;
            uint8_t b;
            uint8_t a;
            
            Color();
            Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xFF);
            
            float getR() const;
            float getG() const;
            float getB() const;
            float getA() const;
        };
    } // namespace video
} // namespace ouzel
