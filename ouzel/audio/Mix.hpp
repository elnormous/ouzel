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
        class Sound;
        class Submix;

        class Mix
        {
            friend Filter;
            friend Listener;
            friend Sound;
            friend Submix;
        public:
            Mix(Audio& initAudio);
            virtual ~Mix();

            Mix(const Mix&) = delete;
            Mix& operator=(const Mix&) = delete;
            Mix(Mix&&) = delete;
            Mix& operator=(Mix&&) = delete;

            uintptr_t getBusId() const { return busId; }

        protected:
            void addInput(Submix* submix);
            void removeInput(Submix* submix);
            void addInput(Sound* sound);
            void removeInput(Sound* sound);
            void addFilter(Filter* filter);
            void removeFilter(Filter* filter);
            void addListener(Listener* listener);
            void removeListener(Listener* listener);

            Audio& audio;
            uintptr_t busId;
            std::vector<Submix*> inputSubmixes;
            std::vector<Sound*> inputSounds;
            std::vector<Filter*> filters;
            std::vector<Listener*> listeners;
        };
    } // namespace audio
} // namespace ouzel

#endif //OUZEL_AUDIO_MIX_HPP
