// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef AUDIO_REVERB_HPP
#define AUDIO_REVERB_HPP

#include "audio/Filter.hpp"

namespace ouzel
{
    namespace audio
    {
        class Reverb final: public Filter
        {
        public:
            Reverb(Audio& initAudio);
            ~Reverb();

            Reverb(const Reverb&) = delete;
            Reverb& operator=(const Reverb&) = delete;
            Reverb(Reverb&&) = delete;
            Reverb& operator=(Reverb&&) = delete;
        };
    } // namespace audio
} // namespace ouzel

#endif // AUDIO_REVERB_HPP
