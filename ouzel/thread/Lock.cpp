// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Lock.hpp"

namespace ouzel
{
    Lock::Lock(Lock&& other)
    {
        mutex = other.mutex;
        other.mutex = nullptr;
    }

    Lock& Lock::operator=(Lock&& other)
    {
        if (mutex) mutex->unlock();
        mutex = other.mutex;
        other.mutex = nullptr;

        return *this;
    }
}
