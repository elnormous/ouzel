// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef AUDIO_SUBMIX_HPP
#define AUDIO_SUBMIX_HPP

#include <cstdint>

namespace ouzel
{
    namespace audio
    {
        class Audio;
        class Filter;

        class Submix final
        {
        public:
            Submix(Audio& initAudio);
            Submix(const Submix&) = delete;
            Submix& operator=(const Submix&) = delete;

            Submix(Submix&&) = delete;
            Submix& operator=(Submix&&) = delete;

            void setOutput(Submix* newOutput);
            void addFilter(Filter* filter);

        private:
            Audio& audio;
            uintptr_t nodeId;
            Submix* output = nullptr;
        };
    } // namespace audio
} // namespace ouzel

#endif // AUDIO_SUBMIX_HPP
