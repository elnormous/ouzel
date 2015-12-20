//
//  CompileConfig.h
//  ouzel
//
//  Created by Elviss Strazdins on 18/12/15.
//  Copyright © 2015 Bool Games. All rights reserved.
//

#pragma once

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
#define BB_PLATFORM_WINDOWS
#endif

#if defined(__APPLE__)
#define BB_PLATFORM_OSX
#endif
