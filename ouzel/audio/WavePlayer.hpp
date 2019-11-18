// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_WAVEPLAYER_HPP
#define OUZEL_AUDIO_WAVEPLAYER_HPP

#include "audio/Source.hpp"
#include "audio/Sound.hpp"

namespace ouzel
{
    namespace audio
    {
        class VorbisData;

        class WavePlayer final: public Source
        {
        public:
            explicit WavePlayer(const Sound* sound)
            {
            }

            void getSamples(uint32_t frames, uint32_t channels, uint32_t sampleRate, std::vector<float>& samples) override
            {
            }

        private:
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_WAVEPLAYER_HPP
