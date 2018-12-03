// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_MIX_HPP
#define OUZEL_AUDIO_MIX_HPP

#include <cstdint>
#include <vector>

namespace ouzel
{
    namespace audio
    {
        class Audio;
        class Filter;
        class Listener;

        class Mix
        {
        public:
            Mix(Audio& initAudio);
            virtual ~Mix();
            Mix(const Mix&) = delete;
            Mix& operator=(const Mix&) = delete;

            Mix(Mix&&) = delete;
            Mix& operator=(Mix&&) = delete;

            uintptr_t getBusId() const { return busId; }
            
            void addFilter(Filter* filter);
            void removeFilter(Filter* filter);

            void addListener(Listener* listener);
            void removeListener(Listener* listener);

        protected:
            Audio& audio;
            uintptr_t busId;
            std::vector<Filter*> filters;
            std::vector<Listener*> listeners;
        };
    } // namespace audio
} // namespace ouzel

#endif //OUZEL_AUDIO_MIX_HPP
