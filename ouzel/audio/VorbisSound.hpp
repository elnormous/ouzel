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
        class VorbisSound final: public Sound
        {
        public:
            VorbisSound(Audio& initAudio, const std::vector<uint8_t>& initData);

        private:
            std::vector<uint8_t> data;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_VORBISDATA_HPP
