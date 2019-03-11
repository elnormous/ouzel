// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_CUE_HPP
#define OUZEL_AUDIO_CUE_HPP

#include "audio/Sound.hpp"

namespace ouzel
{
    namespace audio
    {
        class Cue: public Sound
        {
        public:
            Cue(Audio& initAudio);
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_CUE_HPP
