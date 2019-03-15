// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_CUE_HPP
#define OUZEL_AUDIO_CUE_HPP

#include <vector>
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
                std::vector<Node*> children;
            };

            class Parallel: public Node
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

        private:
            Node* firstNode = nullptr;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_CUE_HPP
