// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#if defined(_MSC_VER)
#include <Windows.h>
#else
#include <pthread.h>
#endif

namespace ouzel
{
    class Condition;

    class Mutex
    {
        friend Condition;
    public:
        Mutex();
        ~Mutex();

        Mutex(const Mutex&) = delete;
        Mutex& operator=(const Mutex&) = delete;

        Mutex(Mutex&&) = delete;
        Mutex& operator=(Mutex&&) = delete;

        bool lock();
        bool tryLock();
        bool unlock();

    protected:
#if defined(_MSC_VER)
        CRITICAL_SECTION criticalSection;
#else
        pthread_mutex_t mutex;
#endif
    };
}
