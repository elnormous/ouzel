// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_VORBISSOUND_HPP
#define OUZEL_AUDIO_VORBISSOUND_HPP

#include <cstdint>
#include <vector>
#include "audio/Sound.hpp"

namespace ouzel
{
    namespace audio
    {
        class VorbisClip final: public Sound
        {
        public:
            VorbisClip(Audio& initAudio, const std::vector<uint8_t>& initData);
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_VORBISSOUND_HPP
