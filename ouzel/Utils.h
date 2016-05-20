// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <limits>

#include "CompileConfig.h"
#if defined(OUZEL_PLATFORM_ANDROID)
#include <cpu-features.h>
#endif

#define OUZEL_UNUSED(x) (void)(x)

namespace ouzel
{
    extern char TEMP_BUFFER[65536];

#if defined(OUZEL_PLATFORM_ANDROID) && defined(OUZEL_SUPPORTS_NEON_CHECK)
    class AnrdoidNEONChecker
    {
    public:
        AnrdoidNEONChecker()
        {
            if (android_getCpuFamily() == ANDROID_CPU_FAMILY_ARM && (android_getCpuFeatures() & ANDROID_CPU_ARM_FEATURE_NEON) != 0)
                neonAvailable = true;
            else
                neonAvailable = false;
        }

        bool isNEONAvailable() const { return neonAvailable; }

    private:
        bool neonAvailable;
    };

    extern AnrdoidNEONChecker anrdoidNEONChecker;
#endif

    template<typename T> size_t vectorDataSize(const typename std::vector<T>& vec)
    {
        return sizeof(T) * vec.size();
    }

    void log(const char* format, ...);
    uint64_t getCurrentMicroSeconds();

    void setArgs(int argc, char* argv[]);
    int getArgc();
    char** getArgv();
    void setArgs(const std::vector<std::string>& args);
    const std::vector<std::string>& getArgs();

    uint32_t random(uint32_t min = 0, uint32_t max = std::numeric_limits<uint32_t>::max());
    float randomf(float min = 0.0f, float max = 1.0f);
}
