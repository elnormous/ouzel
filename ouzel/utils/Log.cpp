// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Log.h"

namespace ouzel
{
#ifndef DEBUG
    Log::Level Log::threshold = Log::Level::INFO;
#else
    Log::Level Log::threshold = Log::Level::ALL;
#endif
}
