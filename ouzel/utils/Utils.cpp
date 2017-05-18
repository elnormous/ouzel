// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <stdio.h>
#include <stdlib.h>
#include "Utils.h"

namespace ouzel
{
#if OUZEL_PLATFORM_ANDROID
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

    #if OUZEL_SUPPORTS_NEON_CHECK
        AnrdoidNEONChecker anrdoidNEONChecker;
    #endif
#endif

    std::mt19937 randomEngine(std::random_device{}());
}
