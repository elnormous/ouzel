// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#include "Utils.hpp"

namespace ouzel
{
#if OUZEL_PLATFORM_ANDROID && OUZEL_SUPPORTS_NEON_CHECK
    AnrdoidNEONChecker anrdoidNEONChecker;
#endif

    std::mt19937 randomEngine(std::random_device{}());
}
