// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <chrono>
#include "thread/Lock.hpp"

namespace ouzel
{
    class Condition final
    {
    public:
        Condition();
        ~Condition();

        Condition(const Condition&) = delete;
        Condition& operator=(const Condition&) = delete;

        Condition(Condition&&) = delete;
        Condition& operator=(Condition&&) = delete;

        bool signal();
        bool broadcast();
        bool wait(Lock& lock);
        bool wait(Lock& lock, std::chrono::steady_clock::duration duration);

    protected:
#if defined(_WIN32)
        CONDITION_VARIABLE conditionVariable;
#else
        pthread_cond_t condition;
#endif
    };
}
