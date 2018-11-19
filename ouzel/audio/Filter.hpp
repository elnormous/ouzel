// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_FILTER_HPP
#define OUZEL_AUDIO_FILTER_HPP

#include "audio/SoundInput.hpp"
#include "audio/SoundOutput.hpp"

namespace ouzel
{
    namespace audio
    {
        class Filter final: public SoundInput, public SoundOutput
        {
        public:
            Filter();

            Filter(const Filter&) = delete;
            Filter& operator=(const Filter&) = delete;

            Filter(Filter&&) = delete;
            Filter& operator=(Filter&&) = delete;

        private:
            uintptr_t nodeId = 0;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_FILTER_HPP
