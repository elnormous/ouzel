// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "thread/Mutex.hpp"

namespace ouzel
{
    class Lock final
    {
    public:
        Lock(Mutex& initMutex):
            mutex(&initMutex)
        {
            mutex->lock();
        }

        ~Lock()
        {
            if (mutex) mutex->unlock();
        }

        Lock(const Lock&) = delete;
        Lock& operator=(const Lock&) = delete;

        Lock(Lock&& other);
        Lock& operator=(Lock&& other);

    private:
        Mutex* mutex = nullptr;
    };
}
