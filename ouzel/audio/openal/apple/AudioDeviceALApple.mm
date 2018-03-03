// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "AudioDeviceALApple.hpp"
#import <AVFoundation/AVFoundation.h>

namespace ouzel
{
    namespace audio
    {
        bool setSessionCategoryApple()
        {
            return [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryAmbient error:nil];
        }
    }
}
