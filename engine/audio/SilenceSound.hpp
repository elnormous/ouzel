// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_SILENCESOUND_HPP
#define OUZEL_AUDIO_SILENCESOUND_HPP

#include "Sound.hpp"

namespace ouzel::audio
{
    class SilenceData;

    class SilenceSound final: public Sound
    {
    public:
        SilenceSound(Audio& initAudio, float initLength = 0.0F);

    private:
        SilenceData* data;
        float length;
    };
}

#endif // OUZEL_AUDIO_SILENCESOUND_HPP
