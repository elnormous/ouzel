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
            class Node
            {

            };

            class Random: public Node
            {

            };

            class Repeat: public Node
            {

            };

            class Sequence: public Node
            {

            };

            class SoundPlayer: public Node
            {

            };

            Cue(Audio& initAudio);
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_CUE_HPP
