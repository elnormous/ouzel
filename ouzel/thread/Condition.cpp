// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#ifdef __APPLE__
#include <sys/time.h>
#endif
#include "Condition.hpp"

namespace ouzel
{
    Condition::Condition()
    {
#if defined(_WIN32)
        InitializeConditionVariable(&conditionVariable);
#else
        pthread_cond_init(&condition, NULL);
#endif
    }

    Condition::~Condition()
    {
#if !defined(_WIN32)
        pthread_cond_destroy(&condition);
#endif
    }

    bool Condition::signal()
    {
#if defined(_WIN32)
        WakeConditionVariable(&conditionVariable);
        return true;
#else
        return pthread_cond_signal(&condition) == 0;
#endif
    }

    bool Condition::broadcast()
    {
#if defined(_WIN32)
        WakeAllConditionVariable(&conditionVariable);
        return true;
#else
        return pthread_cond_broadcast(&condition) == 0;
#endif
    }

    bool Condition::wait(Mutex& mutex)
    {
#if defined(_WIN32)
        return SleepConditionVariableCS(&conditionVariable, &mutex.criticalSection, INFINITE) != 0;
#else
        return pthread_cond_wait(&condition, &mutex.mutex) == 0;
#endif
    }

    bool Condition::wait(Mutex& mutex, std::chrono::steady_clock::duration duration)
    {
#if defined(_WIN32)
        return SleepConditionVariableCS(&conditionVariable, &mutex.criticalSection,
                                        static_cast<DWORD>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count())) != 0;
#else
        static const long NANOSEC_PER_SEC = 1000000000L;
        timespec ts;

#ifdef __APPLE__ // OS X does not have clock_gettime, use gettimeofday
        timeval tv;
        gettimeofday(&tv, nullptr);
        TIMEVAL_TO_TIMESPEC(&tv, &ts);
#else
        clock_gettime(CLOCK_REALTIME, &ts);
#endif

        ts.tv_nsec += std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();

        ts.tv_sec += static_cast<int32_t>(ts.tv_nsec / NANOSEC_PER_SEC);
        ts.tv_nsec %= NANOSEC_PER_SEC;

        return pthread_cond_timedwait(&condition, &mutex.mutex, &ts) == 0;
#endif
    }
}
