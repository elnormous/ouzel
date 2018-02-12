// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Mutex.hpp"

namespace ouzel
{
    Mutex::Mutex()
    {
#if defined(_MSC_VER)
        InitializeCriticalSection(&criticalSection);
#else
        pthread_mutex_init(&mutex, nullptr);
#endif
    }

    Mutex::~Mutex()
    {
#if defined(_MSC_VER)
        DeleteCriticalSection(&criticalSection);
#else
        pthread_mutex_destroy(&mutex);
#endif
    }

    bool Mutex::lock()
    {
#if defined(_MSC_VER)
        EnterCriticalSection(&criticalSection);
        return true;
#else
        return pthread_mutex_lock(&mutex) == 0;
#endif
    }

    bool Mutex::tryLock()
    {
#if defined(_MSC_VER)
        return TryEnterCriticalSection(&criticalSection) != 0;
#else
        return pthread_mutex_trylock(&mutex) == 0;
#endif
    }

    bool Mutex::unlock()
    {
#if defined(_MSC_VER)
        LeaveCriticalSection(&criticalSection);
        return true;
#else
        return pthread_mutex_unlock(&mutex) == 0;
#endif
    }
}
