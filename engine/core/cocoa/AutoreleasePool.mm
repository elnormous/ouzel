// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include "AutoreleasePool.hpp"

namespace ouzel::core::cocoa
{
    AutoreleasePool::AutoreleasePool() noexcept:
        pool([[NSAutoreleasePool alloc] init])
    {
    }

    AutoreleasePool::~AutoreleasePool()
    {
        [pool release];
    }

    AutoreleasePool::AutoreleasePool(const AutoreleasePool& other) noexcept:
        pool([other.pool retain])
    {
    }

    AutoreleasePool::AutoreleasePool(AutoreleasePool&& other) noexcept:
        pool(other.pool)
    {
        other.pool = nil;
    }

    AutoreleasePool& AutoreleasePool::operator=(const AutoreleasePool& other) noexcept
    {
        if (this == &other) return *this;
        [pool release];
        pool = [other.pool retain];
        return *this;
    }

    AutoreleasePool& AutoreleasePool::operator=(AutoreleasePool&& other) noexcept
    {
        if (this == &other) return *this;
        [pool release];
        pool = other.pool;
        other.pool = nil;
        return *this;
    }

    bool AutoreleasePool::operator==(const AutoreleasePool& other) const noexcept
    {
        return pool == other.pool;
    }

    bool AutoreleasePool::operator!=(const AutoreleasePool& other) const noexcept
    {
        return pool != other.pool;
    }
}
