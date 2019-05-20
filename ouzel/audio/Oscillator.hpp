// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_OSCILLATOR_HPP
#define OUZEL_AUDIO_OSCILLATOR_HPP

#include "audio/Sound.hpp"

namespace ouzel
{
    namespace audio
    {
        class OscillatorData;

        class Oscillator final: public Sound
        {
        public:
            enum class Type
            {
                SINE,
                SQUARE,
                SAWTOOTH,
                TRIANGLE
            };

            Oscillator(Audio& initAudio, float initFrequency,
                       Type initType = Type::SINE,
                       float initAmplitude = 0.5F, float initLength = 0.0F);

        private:
            OscillatorData* data;
            Type type;
            float frequency;
            float amplitude;
            float length;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_OSCILLATOR_HPP
