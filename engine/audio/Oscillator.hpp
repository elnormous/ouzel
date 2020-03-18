// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

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
                Sine,
                Square,
                Sawtooth,
                Triangle
            };

            Oscillator(Audio& initAudio, float initFrequency,
                       Type initType = Type::Sine,
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
