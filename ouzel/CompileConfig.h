// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
#define OUZEL_PLATFORM_WINDOWS
#endif

#if defined(__APPLE__)

#include <TargetConditionals.h>

#if TARGET_OS_IPHONE
#define OUZEL_PLATFORM_IOS
#elif TARGET_OS_TV
#define OUZEL_PLATFORM_TVOS
#else
#define OUZEL_PLATFORM_OSX
#endif

#endif
