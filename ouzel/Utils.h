// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <vector>
#include <cstdint>

#define OUZEL_UNUSED(x) (void)(x)

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
}
