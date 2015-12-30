// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace ouzel
{
    extern char TEMP_BUFFER[65536];
    
    template <typename T> int sgn(T val)
    {
        return (T(0) < val) - (val < T(0));
    }

    template<typename T> size_t vectorDataSize(const typename std::vector<T>& vec)
    {
        return sizeof(T) * vec.size();
    }

    void log(const char* format, ...);
    uint64_t getCurrentMicroSeconds();
}
