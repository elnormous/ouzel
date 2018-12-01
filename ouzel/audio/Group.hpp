// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef AUDIO_SUBMIX_HPP
#define AUDIO_SUBMIX_HPP

#include "audio/Bus.hpp"

namespace ouzel
{
    namespace audio
    {
        class Group final: public Bus
        {
        public:
            Group(Audio& initAudio);
            Group(const Group&) = delete;
            Group& operator=(const Group&) = delete;

            Group(Group&&) = delete;
            Group& operator=(Group&&) = delete;

            void setOutput(Bus* newOutput);

        private:
            Bus* output = nullptr;
        };
    } // namespace audio
} // namespace ouzel

#endif // AUDIO_SUBMIX_HPP
