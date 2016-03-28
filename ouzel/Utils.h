// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <climits>

#define OUZEL_UNUSED(x) (void)(x)

#define RANDOM_MINUS1_1() ((2.0f * (static_cast<float>(random()) / UINT_MAX)) - 1.0f) // Returns a random float between -1 and 1.
#define RANDOM_0_1()      (static_cast<float>(random()) / UINT_MAX) // Returns a random float between 0 and 1.

namespace ouzel
{
    extern char TEMP_BUFFER[65536];

    template<typename T> size_t vectorDataSize(const typename std::vector<T>& vec)
    {
        return sizeof(T) * vec.size();
    }

    void log(const char* format, ...);
    uint64_t getCurrentMicroSeconds();

    void setArgs(const std::vector<std::string>& args);
    const std::vector<std::string>& getArgs();

    uint32_t random(uint32_t min = 0, uint32_t max = UINT_MAX);
}
