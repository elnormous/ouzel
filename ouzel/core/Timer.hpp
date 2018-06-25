// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#pragma once

namespace ouzel
{
    class Engine;

    class Timer
    {
        friend Engine;
    public:
        ~Timer() = default;

        Timer(const Timer&) = delete;
        Timer& operator=(const Timer&) = delete;

        Timer(Timer&&) = delete;
        Timer& operator=(Timer&&) = delete;

        void update(float delta);

    protected:
        Timer();
    };
}
