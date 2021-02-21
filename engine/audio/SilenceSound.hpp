// Ouzel by Elviss Strazdins

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
