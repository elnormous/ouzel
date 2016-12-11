// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Size2.h"
#include "Size3.h"

namespace ouzel
{
    Size2::Size2(const Size3& s):
        width(s.width),
        height(s.height)
    {
    }

    Size2& Size2::operator=(const Size3& size)
    {
        width = size.width;
        height = size.height;
        return *this;
    }
}
