// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_FILTER_HPP
#define OUZEL_AUDIO_FILTER_HPP

#include <cstdint>

namespace ouzel
{
    namespace audio
    {
        class Audio;
        class Mix;

        class Filter
        {
            friend Mix;
        public:
            Filter(Audio& initAudio,
                   uintptr_t initProcessorId);
            virtual ~Filter();

            Filter(const Filter&) = delete;
            Filter& operator=(const Filter&) = delete;
            Filter(Filter&&) = delete;
            Filter& operator=(Filter&&) = delete;

            uintptr_t getProcessorId() const { return processorId; }

            void setMix(Mix* newMix);

        protected:
            Audio& audio;
            uintptr_t processorId = 0;
            Mix* mix = nullptr;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_FILTER_HPP
