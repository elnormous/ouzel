// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "AudioALApple.h"
#if OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
#import <AVFoundation/AVFoundation.h>
#endif

namespace ouzel
{
    namespace audio
    {
        bool AudioALApple::init()
        {
            if (!AudioAL::init())
            {
                return false;
            }

#if OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
            [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryAmbient error:Nil];
#endif

            return true;
        }
    }
}
