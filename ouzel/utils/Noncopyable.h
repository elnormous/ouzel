// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

namespace ouzel
{
    class Noncopyable
    {
    protected:
        Noncopyable() = default;
        ~Noncopyable() = default;

        Noncopyable(const Noncopyable&) = delete;
        Noncopyable& operator=(const Noncopyable&) = delete;

        Noncopyable(const Noncopyable&&) = delete;
        Noncopyable& operator=(const Noncopyable&&) = delete;
    };
}
