// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_ALSA

#include "AudioALSA.h"
#include "utils/Log.h"

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

            return true;
        }
    } // namespace audio
} // namespace ouzel

#endif
