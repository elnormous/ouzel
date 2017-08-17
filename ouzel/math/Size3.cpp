// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Size3.hpp"
#include "Size2.hpp"

namespace ouzel
{
    Size3::Size3(const Size2& s):
        width(s.width), height(s.height)
    {
    }

    Size3& Size3::operator=(const Size2& s)
    {
        width = s.width;
        height = s.height;
        depth = 0.0f;
        return *this;
    }
}
