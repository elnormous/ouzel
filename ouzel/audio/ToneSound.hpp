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
            ToneSound();
            explicit ToneSound(const std::vector<uint8_t>& initData);

            std::shared_ptr<Stream> createStream() override;

        private:
            void readData(Stream* stream, uint32_t frames, std::vector<float>& result) override;

            std::vector<float> data;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_TONEDATA_HPP
