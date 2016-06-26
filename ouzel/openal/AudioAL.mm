// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"
#if OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
#import <AVFoundation/AVFoundation.h>
#endif

#include "AudioAL.h"
#include "SoundDataAL.h"
#include "SoundAL.h"
#include "utils/Utils.h"

namespace ouzel
{
    namespace audio
    {
        AudioAL::AudioAL():
            Audio(Driver::OPENAL)
        {
        }

        AudioAL::~AudioAL()
        {
            if (context)
            {
                alcDestroyContext(context);
            }

            if (device)
            {
                alcCloseDevice(device);
            }
        }

        void AudioAL::free()
        {
            Audio::free();

            if (context)
            {
                alcDestroyContext(context);
                context = nullptr;
            }

            if (device)
            {
                alcCloseDevice(device);
                device = nullptr;
            }
        }

        bool AudioAL::init()
        {
            if (!Audio::init())
            {
                return false;
            }

            free();

#if OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
            [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryAmbient error:Nil];
#endif

            device = alcOpenDevice(NULL);

            context = alcCreateContext(device, NULL);
            alcMakeContextCurrent(context);

            ready = true;
            
            return true;
        }

        SoundDataPtr AudioAL::createSoundData()
        {
            SoundDataPtr soundData(new SoundDataAL());
            return soundData;
        }

        SoundPtr AudioAL::createSound()
        {
            SoundPtr sound(new SoundAL());
            return sound;
        }
    } // namespace audio
} // namespace ouzel
