// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_CONTAINERS_HPP
#define OUZEL_AUDIO_CONTAINERS_HPP

#include <vector>
#include "audio/Sound.hpp"

namespace ouzel
{
    namespace audio
    {
        class Track
        {
        public:
            Sound* sound = nullptr;
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
