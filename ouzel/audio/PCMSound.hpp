// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_PCMSOUND_HPP
#define OUZEL_AUDIO_PCMSOUND_HPP

#include <cstdint>
#include <vector>
#include "audio/Sound.hpp"

namespace ouzel
{
    namespace audio
    {
        class PCMSound final: public Sound
        {
        public:
            PCMSound(Audio& initAudio, uint16_t channels, uint32_t sampleRate,
                     const std::vector<float>& samples);
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_PCMSOUND_HPP
