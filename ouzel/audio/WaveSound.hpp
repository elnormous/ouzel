// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_WAVEDATA_HPP
#define OUZEL_AUDIO_WAVEDATA_HPP

#include <cstdint>
#include <vector>
#include "audio/Sound.hpp"

namespace ouzel
{
    namespace audio
    {
        class WaveSound final: public Sound
        {
        public:
            WaveSound(Audio& initAudio, const std::vector<uint8_t>& initData);

        private:
            std::vector<float> data;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_WAVEDATA_HPP
