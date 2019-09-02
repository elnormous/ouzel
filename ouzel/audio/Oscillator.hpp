// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_OSCILLATOR_HPP
#define OUZEL_AUDIO_OSCILLATOR_HPP

#include "audio/Source.hpp"

namespace ouzel
{
    namespace audio
    {
        class Oscillator final: public Source
        {
        public:
            enum class Type
            {
                Sine,
                Square,
                Sawtooth,
                Triangle
            };

            Oscillator(float initFrequency, Type initType = Type::Sine,
                       float initAmplitude = 0.5F, float initLength = 0.0F);

            void getSamples(uint32_t frames, uint16_t channels, uint32_t sampleRate, std::vector<float>& samples);

        private:
            Type type;
            float frequency;
            float amplitude;
            float length;
            uint32_t position = 0;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_OSCILLATOR_HPP
