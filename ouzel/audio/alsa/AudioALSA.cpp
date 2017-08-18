// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_ALSA

#include "AudioALSA.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace audio
    {
        AudioALSA::AudioALSA():
            Audio(Audio::Driver::ALSA)
        {
        }

        AudioALSA::~AudioALSA()
        {
        }

        bool AudioALSA::init(bool debugAudio)
        {
            if (!Audio::init(debugAudio))
            {
                return false;
            }

            int err;
            if ((err = snd_pcm_open(&playbackHandle, "", SND_PCM_STREAM_PLAYBACK, 0)) < 0)
            {
                Log(Log::Level::ERR) << "Failed to initialize ALSA, error: " << err;
                return false;
            }          

            return true;
        }
    } // namespace audio
} // namespace ouzel

#endif
