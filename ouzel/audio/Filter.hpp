// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_FILTER_HPP
#define OUZEL_AUDIO_FILTER_HPP

#include "audio/Source.hpp"
#include "audio/Destination.hpp"

namespace ouzel
{
    namespace audio
    {
        class Audio;

        class Filter final: public Source, public Destination
        {
        public:
            Filter(Audio& initAudio);
            ~Filter();

            Filter(const Filter&) = delete;
            Filter& operator=(const Filter&) = delete;

            Filter(Filter&&) = delete;
            Filter& operator=(Filter&&) = delete;

        private:
            Audio& audio;
            uintptr_t nodeId = 0;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_FILTER_HPP
