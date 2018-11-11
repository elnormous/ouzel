// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_SOUNDDATAVORBIS_HPP
#define OUZEL_AUDIO_SOUNDDATAVORBIS_HPP

#include <cstdint>
#include <vector>
#include "audio/SoundData.hpp"

namespace ouzel
{
    namespace audio
    {
        class SoundDataVorbis final: public SoundData
        {
        public:
            SoundDataVorbis();
            explicit SoundDataVorbis(const std::vector<uint8_t>& initData);

            std::shared_ptr<Stream> createStream() override;

        private:
            void readData(Stream* stream, uint32_t frames, std::vector<float>& result) override;

            std::vector<uint8_t> data;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_SOUNDDATAVORBIS_HPP
