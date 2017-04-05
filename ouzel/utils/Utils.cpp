// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <stdio.h>
#include "Utils.h"

namespace ouzel
{
#if OUZEL_PLATFORM_ANDROID
    static char TEMP_BUFFER[256];

    std::string toString(int val)
    {
        snprintf(TEMP_BUFFER, sizeof(TEMP_BUFFER), "%d", val);
        return std::string(TEMP_BUFFER);
    }

    std::string toString(unsigned val)
    {
        snprintf(TEMP_BUFFER, sizeof(TEMP_BUFFER), "%u", val);
        return std::string(TEMP_BUFFER);
    }

    std::string toString(long val)
    {
        snprintf(TEMP_BUFFER, sizeof(TEMP_BUFFER), "%ld", val);
        return std::string(TEMP_BUFFER);
    }

    std::string toString(unsigned long val)
    {
        snprintf(TEMP_BUFFER, sizeof(TEMP_BUFFER), "%lu", val);
        return std::string(TEMP_BUFFER);
    }

    std::string toString(long long val)
    {
        snprintf(TEMP_BUFFER, sizeof(TEMP_BUFFER), "%lld", val);
        return std::string(TEMP_BUFFER);
    }

    std::string toString(unsigned long long val)
    {
        snprintf(TEMP_BUFFER, sizeof(TEMP_BUFFER), "%llu", val);
        return std::string(TEMP_BUFFER);
    }

    std::string toString(float val)
    {
        snprintf(TEMP_BUFFER, sizeof(TEMP_BUFFER), "%f", val);
        return std::string(TEMP_BUFFER);
    }

    std::string toString(double val)
    {
        snprintf(TEMP_BUFFER, sizeof(TEMP_BUFFER), "%f", val);
        return std::string(TEMP_BUFFER);
    }

    std::string toString(long double val)
    {
        snprintf(TEMP_BUFFER, sizeof(TEMP_BUFFER), "%Lf", val);
        return std::string(TEMP_BUFFER);
    }

    #if OUZEL_SUPPORTS_NEON_CHECK
        AnrdoidNEONChecker anrdoidNEONChecker;
    #endif
#endif

    std::mt19937 randomEngine(std::random_device{}());
}
