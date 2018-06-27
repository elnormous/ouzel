// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

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

        void signal();
        void broadcast();
        bool wait(Lock& lock);
        bool wait(Lock& lock, std::chrono::steady_clock::duration duration);

    private:
#if defined(_WIN32)
        CONDITION_VARIABLE conditionVariable;
#else
        pthread_cond_t condition;
        bool initialized = false;
#endif
    };
}
