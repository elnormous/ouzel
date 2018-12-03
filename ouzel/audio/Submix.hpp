// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_SUBMIX_HPP
#define OUZEL_AUDIO_SUBMIX_HPP

#include "audio/Mix.hpp"

namespace ouzel
{
    namespace audio
    {
        class Submix final: public Mix
        {
        public:
            Submix(Audio& initAudio);
            Submix(const Submix&) = delete;
            Submix& operator=(const Submix&) = delete;

            Submix(Submix&&) = delete;
            Submix& operator=(Submix&&) = delete;

            void setOutput(Mix* newOutput);

        private:
            Mix* output = nullptr;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_SUBMIX_HPP
