// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_TONEDATA_HPP
#define OUZEL_AUDIO_TONEDATA_HPP

#include <cstdint>
#include <vector>
#include "audio/Sound.hpp"

namespace ouzel
{
    namespace audio
    {
        class ToneSound final: public Sound
        {
        public:
            enum class Type
            {
                SINE,
                SQUARE,
                SAWTOOTH,
                TRIANGLE
            };

            ToneSound(Audio& initAudio, float initFrequency, Type initType = Type::SINE, float initAmplitude = 0.5F);

        private:
            float frequency;
            Type type;
            float amplitude;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_TONEDATA_HPP
