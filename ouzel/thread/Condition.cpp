// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Condition.hpp"

namespace ouzel
{
    Condition::Condition()
    {
#if defined(_MSC_VER)
        InitializeConditionVariable(&conditionVariable);
#else
        pthread_cond_init(&condition, NULL);
#endif
    }

    Condition::~Condition()
    {
#if !defined(_MSC_VER)
        pthread_cond_destroy(&condition);
#endif
    }

    bool Condition::signal()
    {
#if defined(_MSC_VER)
        WakeConditionVariable(&conditionVariable);
        return true;
#else
        return pthread_cond_signal(&condition) == 0;
#endif
    }

    bool Condition::broadcast()
    {
#if defined(_MSC_VER)
        WakeAllConditionVariable(&conditionVariable);
        return true;
#else
        return pthread_cond_broadcast(&condition) == 0;
#endif
    }

    bool Condition::wait(Mutex& mutex)
    {
#if defined(_MSC_VER)
        return SleepConditionVariableCS(&conditionVariable, &mutex.criticalSection, INFINITE);
#else
        return pthread_cond_wait(&condition, &mutex.mutex) == 0;
#endif
    }

    bool Condition::wait(Mutex& mutex, std::chrono::steady_clock::duration duration)
    {
#if defined(_MSC_VER)
        return SleepConditionVariableCS(&conditionVariable, &mutex.criticalSection, std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
#else
        static const long NSEC_PER_SEC = 1000000000L;
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_nsec += std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();

        ts.tv_sec += static_cast<int32_t>(ts.tv_nsec / NSEC_PER_SEC);
        ts.tv_nsec %= NSEC_PER_SEC;

        return pthread_cond_timedwait(&condition, &mutex.mutex, &ts) == 0;
#endif
    }
}
