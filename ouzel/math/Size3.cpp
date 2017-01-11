// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Size3.h"
#include "Size2.h"

namespace ouzel
{
    Size3::Size3(const Size2& s)
    {
        v[0] = s.v[0];
        v[1] = s.v[1];
        v[2] = 0.0f;
    }

    Size3& Size3::operator=(const Size2& s)
    {
        v[0] = s.v[0];
        v[1] = s.v[1];
        v[2] = 0.0f;

        return *this;
    }
}
