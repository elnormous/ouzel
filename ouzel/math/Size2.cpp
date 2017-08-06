// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Size2.hpp"
#include "Size3.hpp"

namespace ouzel
{
    Size2::Size2(const Size3& s)
    {
        v[0] = s.v[0];
        v[1] = s.v[1];
    }

    Size2& Size2::operator=(const Size3& s)
    {
        v[0] = s.v[0];
        v[1] = s.v[1];
        return *this;
    }
}
