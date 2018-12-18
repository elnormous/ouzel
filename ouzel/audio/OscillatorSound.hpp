// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_OSCILLATORSOUND_HPP
#define OUZEL_AUDIO_OSCILLATORSOUND_HPP

#include "audio/Sound.hpp"

namespace ouzel
{
    namespace audio
    {
        class OscillatorSound final: public Sound
        {
        public:
            enum class Type
            {
                SINE,
                SQUARE,
                SAWTOOTH,
                TRIANGLE
            };

            OscillatorSound(Audio& initAudio, float initFrequency,
                            Type initType = Type::SINE,
                            float initAmplitude = 0.5F, float initLength = 0.0F);

        private:
            Type type;
            float frequency;
            float amplitude;
            float length;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_OSCILLATORSOUND_HPP
