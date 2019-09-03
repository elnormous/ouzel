// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_VORBISCLIP_HPP
#define OUZEL_AUDIO_VORBISCLIP_HPP

#include <cstdint>
#include <vector>
#include "audio/Source.hpp"

namespace ouzel
{
    namespace audio
    {
        class VorbisData;

        class VorbisClip final: public Source
        {
        public:
            VorbisClip(const std::vector<uint8_t>& initData);

            void getSamples(uint32_t frames, uint16_t channels, uint32_t sampleRate, std::vector<float>& samples) override;

        private:
            VorbisData* data;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_VORBISCLIP_HPP
