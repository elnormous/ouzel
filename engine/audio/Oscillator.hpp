#ifndef FLUXION_AUDIO_OSCILLATOR_HPP
#define FLUXION_AUDIO_OSCILLATOR_HPP

#include "Sound.hpp"

namespace fluxion::audio
{
    class OscillatorData;

    class Oscillator final: public Sound
    {
    public:
        enum class Type
        {
            sine,
            square,
            sawtooth,
            triangle
        };

        Oscillator(Audio& initAudio, float initFrequency,
                   Type initType = Type::sine,
                   float initAmplitude = 0.5F, float initLength = 0.0F);

    private:
        OscillatorData* data;
        Type type;
        float frequency;
        float amplitude;
        float length;
    };
}

#endif // FLUXION_AUDIO_OSCILLATOR_HPP
