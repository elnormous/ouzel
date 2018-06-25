// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#include "Mutex.hpp"
#include "utils/Errors.hpp"

namespace ouzel
{
    Mutex::Mutex()
    {
#if defined(_WIN32)
        InitializeCriticalSection(&criticalSection);
#else
        if (pthread_mutex_init(&mutex, nullptr) != 0)
            throw ThreadError("Failed to initialize mutex");

        initialized = true;
#endif
    }

    Mutex::~Mutex()
    {
#if defined(_WIN32)
        DeleteCriticalSection(&criticalSection);
#else
        if (initialized) pthread_mutex_destroy(&mutex);
#endif
    }

    void Mutex::lock()
    {
#if defined(_WIN32)
        EnterCriticalSection(&criticalSection);
#else
        if (pthread_mutex_lock(&mutex) != 0)
            throw ThreadError("Failed to lock mutex");
#endif
    }

    bool Mutex::tryLock()
    {
#if defined(_WIN32)
        return TryEnterCriticalSection(&criticalSection) != 0;
#else
        return pthread_mutex_trylock(&mutex) == 0;
#endif
    }

    void Mutex::unlock()
    {
#if defined(_WIN32)
        LeaveCriticalSection(&criticalSection);
#else
        if (pthread_mutex_unlock(&mutex) != 0)
            throw ThreadError("Failed to unlock mutex");
#endif
    }
}
