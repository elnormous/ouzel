// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_VORBISDATA_HPP
#define OUZEL_AUDIO_VORBISDATA_HPP

#include <cstdint>
#include <vector>
#include "audio/Sound.hpp"

namespace ouzel
{
    namespace audio
    {
        class VorbisData final: public Sound
        {
        public:
            VorbisData();
            explicit VorbisData(const std::vector<uint8_t>& initData);

            std::shared_ptr<Stream> createStream() override;

        private:
            void readData(Stream* stream, uint32_t frames, std::vector<float>& result) override;

            std::vector<uint8_t> data;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_VORBISDATA_HPP
