// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <stdio.h>
#include <stdlib.h>
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

    long stringToLong(const std::string& val)
    {
        return strtol(val.c_str(), nullptr, 0);
    }

    long long stringToLongLong(const std::string& val)
    {
        return strtoll(val.c_str(), nullptr, 0);
    }

    float stringToFloat(const std::string& val)
    {
        return strtof(val.c_str(), nullptr);
    }

    double stringToDouble(const std::string& val)
    {
        return strtod(val.c_str(), nullptr);
    }

    long double stringToLongDouble(const std::string& val)
    {
        return strtold(val.c_str(), nullptr);
    }

    #if OUZEL_SUPPORTS_NEON_CHECK
        AnrdoidNEONChecker anrdoidNEONChecker;
    #endif
#endif

    std::mt19937 randomEngine(std::random_device{}());
}
