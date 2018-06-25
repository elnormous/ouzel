// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Utils.hpp"

namespace ouzel
{
#if OUZEL_PLATFORM_ANDROID && OUZEL_SUPPORTS_NEON_CHECK
    AnrdoidNEONChecker anrdoidNEONChecker;
#endif

    std::mt19937 randomEngine(std::random_device{}());
}
