// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_BUS_HPP
#define OUZEL_AUDIO_BUS_HPP

#include <cstdint>

namespace ouzel
{
    namespace audio
    {
        class Audio;
        class Filter;
        class Listener;

        class Bus
        {
        public:
            Bus(Audio& initAudio);
            Bus(const Bus&) = delete;
            Bus& operator=(const Bus&) = delete;

            Bus(Bus&&) = delete;
            Bus& operator=(Bus&&) = delete;

            void addFilter(Filter* filter);
            void removeFilter(Filter* filter);

            void addListener(Listener* listener);
            void removeListener(Listener* listener);
            
        protected:
            Audio& audio;
            uintptr_t objectId;
        };
    } // namespace audio
} // namespace ouzel

#endif //OUZEL_AUDIO_BUS_HPP
