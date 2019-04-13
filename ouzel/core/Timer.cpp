// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "Timer.hpp"

namespace ouzel
{
    Timer::Timer()
    {
    }

    void Timer::update(float delta)
    {
        time += delta;
    }
}
