#ifndef FLUXION_AUDIO_SILENCESOUND_HPP
#define FLUXION_AUDIO_SILENCESOUND_HPP

#include "Sound.hpp"

namespace fluxion::audio
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

#endif // FLUXION_AUDIO_SILENCESOUND_HPP
