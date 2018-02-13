// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif
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
        static const long NANOSEC_PER_SEC = 1000000000L;
        struct timespec ts;

#ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
        clock_serv_t cclock;
        mach_timespec_t mts;
        host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
        clock_get_time(cclock, &mts);
        mach_port_deallocate(mach_task_self(), cclock);
        ts.tv_sec = mts.tv_sec;
        ts.tv_nsec = mts.tv_nsec;
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
