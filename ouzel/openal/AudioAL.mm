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
        bool AudioAL::checkOpenALErrors(bool logError)
        {
            bool gotError = false;

            while (ALenum error = alGetError() != AL_NO_ERROR)
            {
                if (logError)
                {
                    const char* errorStr = "Unknown error";

                    switch (error)
                    {
                        case AL_INVALID_NAME: errorStr = "AL_INVALID_NAME"; break;
                        case AL_INVALID_ENUM: errorStr = "GL_INVALID_ENUM"; break;
                        case AL_INVALID_VALUE: errorStr = "GL_INVALID_VALUE"; break;
                        case AL_INVALID_OPERATION: errorStr = "GL_INVALID_OPERATION"; break;
                        case AL_OUT_OF_MEMORY: errorStr = "GL_OUT_OF_MEMORY"; break;
                    }

                    log("OpenAL error: %s (%x)", errorStr, error);
                }

                gotError = true;
            }
            
            return gotError;
        }

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

                if (AudioAL::checkOpenALErrors())
                {
                    log("Failed to delete OpenAL context");
                }
            }

            if (device)
            {
                alcCloseDevice(device);
                device = nullptr;

                if (AudioAL::checkOpenALErrors())
                {
                    log("Failed to close OpenAL device");
                }
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

            if (checkOpenALErrors())
            {
                log("Failed to create OpenAL device");
                return false;
            }

            context = alcCreateContext(device, NULL);

            if (checkOpenALErrors())
            {
                log("Failed to create OpenAL context");
                return false;
            }

            alcMakeContextCurrent(context);

            if (checkOpenALErrors())
            {
                log("Failed to make OpenAL context current");
                return false;
            }

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
