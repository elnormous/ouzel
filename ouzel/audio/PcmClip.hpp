// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_PCMCLIP_HPP
#define OUZEL_AUDIO_PCMCLIP_HPP

#include <cstdint>
#include <vector>
#include "audio/Source.hpp"

namespace ouzel
{
    namespace audio
    {
        class PcmData;

        class PcmClip final: public Source
        {
        public:
            PcmClip(uint16_t channels, uint32_t sampleRate,
                    const std::vector<float>& samples);

            void getSamples(uint32_t frames, uint16_t channels, uint32_t sampleRate, std::vector<float>& samples) override;

        private:
            PcmData* data;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_PCMCLIP_HPP
