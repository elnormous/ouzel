// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
#define BB_PLATFORM_WINDOWS
#endif

#if defined(__APPLE__)
#define BB_PLATFORM_OSX
#endif
