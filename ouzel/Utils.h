// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>

namespace ouzel
{
    extern char TEMP_BUFFER[65536];
    
    template <typename T> int sgn(T val)
    {
        return (T(0) < val) - (val < T(0));
    }

    void log(const char* format, ...);
    uint64_t getCurrentMicroSeconds();
}
