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
            };

            class Parallel: public Node
            {
                std::vector<Node*> children;
            };

            class Random: public Node
            {
                std::vector<Node*> children;
            };

            class Repeat: public Node
            {
                std::vector<Node*> children;
            };

            class Sequence: public Node
            {
                std::vector<Node*> children;
            };

            class SoundPlayer: public Node
            {
                Sound* sound = nullptr;
            };

            Cue(Audio& initAudio);

        private:
            Node* firstNode = nullptr;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_CUE_HPP
