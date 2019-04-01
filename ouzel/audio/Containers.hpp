// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_CONTAINERS_HPP
#define OUZEL_AUDIO_CONTAINERS_HPP

#include <algorithm>
#include <vector>
#include "audio/Source.hpp"

namespace ouzel
{
    namespace audio
    {
        class Container: public Source
        {
        public:
            void addSource(Source* source)
            {
                auto i = std::find(sources.begin(), sources.end(), source);

                if (i == sources.end())
                    sources.push_back(source);
            }

            void removeSource(Source* source)
            {
                auto i = std::find(sources.begin(), sources.end(), source);

                if (i != sources.end())
                    sources.erase(i);
            }

        private:
            std::vector<Source*> sources;
        };

        class Parallel final: public Container
        {
        public:
        };

        class Random final: public Container
        {
        public:
        };

        class Sequence final: public Container
        {
        public:
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_CONTAINERS_HPP
