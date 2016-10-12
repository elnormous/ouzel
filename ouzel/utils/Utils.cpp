// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <random>
#include "Utils.h"

namespace ouzel
{
#if OUZEL_PLATFORM_ANDROID && OUZEL_SUPPORTS_NEON_CHECK
    AnrdoidNEONChecker anrdoidNEONChecker;
#endif

    static std::mt19937 engine(std::random_device{}());

    uint32_t random(uint32_t min, uint32_t max)
    {
        return std::uniform_int_distribution<uint32_t>{min, max}(engine);
    }

    float randomf(float min, float max)
    {
        float diff = max - min;

        float rand = static_cast<float>(engine()) / engine.max();

        return rand * diff + min;
    }
}
