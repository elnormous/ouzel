// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifdef __APPLE__
#include <sys/time.h>
#endif
#include "Condition.hpp"
#include "utils/Errors.hpp"

namespace ouzel
{
    Condition::Condition()
    {
#if defined(_WIN32)
        InitializeConditionVariable(&conditionVariable);
#else
        if (pthread_cond_init(&condition, NULL) != 0)
            throw ThreadError("Failed to initialize condition");

        initialized = true;
#endif
    }

    Condition::~Condition()
    {
#if !defined(_WIN32)
        if (initialized) pthread_cond_destroy(&condition);
#endif
    }

    void Condition::signal()
    {
#if defined(_WIN32)
        WakeConditionVariable(&conditionVariable);
#else
        if (pthread_cond_signal(&condition) != 0)
            throw ThreadError("Failed to signal condition variable");
#endif
    }

    void Condition::broadcast()
    {
#if defined(_WIN32)
        WakeAllConditionVariable(&conditionVariable);
#else
        if (pthread_cond_broadcast(&condition) != 0)
            throw ThreadError("Failed to broadcast condition variable");
#endif
    }

    bool Condition::wait(Lock& lock)
    {
        if (!lock.mutex) return false;
#if defined(_WIN32)
        return SleepConditionVariableCS(&conditionVariable, &lock.mutex->criticalSection, INFINITE) != 0;
#else
        return pthread_cond_wait(&condition, &lock.mutex->mutex) == 0;
#endif
    }

    bool Condition::wait(Lock& lock, std::chrono::steady_clock::duration duration)
    {
        if (!lock.mutex) return false;
#if defined(_WIN32)
        return SleepConditionVariableCS(&conditionVariable, &lock.mutex->criticalSection,
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

        return pthread_cond_timedwait(&condition, &lock.mutex->mutex, &ts) == 0;
#endif
    }
}
