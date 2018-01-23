// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "utils/Noncopyable.hpp"

namespace ouzel
{
    class Engine;

    class Timer: public Noncopyable
    {
        friend Engine;
    public:
        void update(float delta);

    protected:
        Timer();
    };
}
