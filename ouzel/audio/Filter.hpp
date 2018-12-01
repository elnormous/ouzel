// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_FILTER_HPP
#define OUZEL_AUDIO_FILTER_HPP

#include <cstdint>

namespace ouzel
{
    namespace audio
    {
        class Audio;

        class Filter
        {
        public:
            Filter(Audio& initAudio);
            virtual ~Filter();

            Filter(const Filter&) = delete;
            Filter& operator=(const Filter&) = delete;

            Filter(Filter&&) = delete;
            Filter& operator=(Filter&&) = delete;

        protected:
            Audio& audio;
            uintptr_t objectId = 0;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_FILTER_HPP
