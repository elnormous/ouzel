// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "AudioALIOS.h"
#import <AVFoundation/AVFoundation.h>

namespace ouzel
{
    namespace audio
    {
        bool AudioALIOS::init()
        {
            if (!AudioAL::init())
            {
                return false;
            }

            [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryAmbient error:Nil];

            return true;
        }
    }
}
