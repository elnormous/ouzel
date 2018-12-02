// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef AUDIO_MIXER_HPP
#define AUDIO_MIXER_HPP

#include <cstdint>
#include <vector>

namespace ouzel
{
    namespace audio
    {
        class Mixer
        {
        public:
            Mixer() {}

            Mixer(const Mixer&) = delete;
            Mixer& operator=(const Mixer&) = delete;

            Mixer(Mixer&&) = delete;
            Mixer& operator=(Mixer&&) = delete;

            void getData(uint32_t frames, uint32_t sampleRate, std::vector<uint8_t>& result);
        };
    } // namespace audio
} // namespace ouzel

#endif // AUDIO_MIXER_HPP
