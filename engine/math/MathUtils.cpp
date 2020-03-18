// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include "MathUtils.hpp"

namespace ouzel
{
#if defined(__ANDROID__) && defined(__ARM_NEON__) && defined(__arm__)
    // NEON support must be checked at runtime on 32-bit Android
    const AnrdoidNeonChecker isSimdAvailable;
#endif
}
