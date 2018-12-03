// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_MIX_HPP
#define OUZEL_AUDIO_MIX_HPP

#include <cstdint>

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
            Mix(const Mix&) = delete;
            Mix& operator=(const Mix&) = delete;

            Mix(Mix&&) = delete;
            Mix& operator=(Mix&&) = delete;

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

#endif //OUZEL_AUDIO_MIX_HPP
