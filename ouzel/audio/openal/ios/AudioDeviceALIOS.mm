// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_PLATFORM_IOS && OUZEL_SUPPORTS_OPENAL

#include "AudioDeviceALIOS.hpp"
#import <AVFoundation/AVFoundation.h>

namespace ouzel
{
    namespace audio
    {
        bool AudioDeviceALIOS::init(bool debugAudio)
        {
            if (!AudioDeviceAL::init(debugAudio))
            {
                return false;
            }

            [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryAmbient error:nil];

            return true;
        }
    }
}

#endif
