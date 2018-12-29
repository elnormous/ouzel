// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_SOUNDCUE_HPP
#define OUZEL_AUDIO_SOUNDCUE_HPP

#include "audio/Sound.hpp"

namespace ouzel
{
    namespace audio
    {
        class SoundCue final: public Sound
        {
        public:
            SoundCue(Audio& initAudio);
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_SOUNDCUE_HPP
