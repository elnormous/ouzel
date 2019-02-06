// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_CONTAINERS_HPP
#define OUZEL_AUDIO_CONTAINERS_HPP

#include <cstdint>
#include <vector>
#include "audio/Effect.hpp"
#include "audio/Sound.hpp"

namespace ouzel
{
    namespace audio
    {
        class Track
        {
        public:
            Track(Sound* initSound, uint32_t initOrder = 0):
                sound(initSound), order(initOrder)
            {

            }

            Sound* getSound() const { return sound; }
            uint32_t getOrder() const { return order; }

        private:
            Sound* sound = nullptr;
            uint32_t order;
        };

        class Container
        {
        public:
            Container(const std::initializer_list<Track>& initTracks):
                tracks(initTracks)
            {
            }

            void addTrack(const Track& track)
            {
                tracks.push_back(track);
            }

        private:
            std::vector<Track> tracks;
        };

        class ParallelContainer final: public Container
        {
        public:
        };

        class RandomContainer final: public Container
        {
        public:
        };

        class SequenceContainer final: public Container
        {
        public:
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_CONTAINERS_HPP
