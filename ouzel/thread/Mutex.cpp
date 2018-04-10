// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Mutex.hpp"

namespace ouzel
{
    Mutex::Mutex()
    {
#if defined(_WIN32)
        InitializeCriticalSection(&criticalSection);
#else
        pthread_mutex_init(&mutex, nullptr);
#endif
    }

    Mutex::~Mutex()
    {
#if defined(_WIN32)
        DeleteCriticalSection(&criticalSection);
#else
        pthread_mutex_destroy(&mutex);
#endif
    }

    bool Mutex::lock()
    {
#if defined(_WIN32)
        EnterCriticalSection(&criticalSection);
        return true;
#else
        return pthread_mutex_lock(&mutex) == 0;
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

    bool Mutex::unlock()
    {
#if defined(_WIN32)
        LeaveCriticalSection(&criticalSection);
        return true;
#else
        return pthread_mutex_unlock(&mutex) == 0;
#endif
    }
}
