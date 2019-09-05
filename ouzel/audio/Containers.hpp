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
        class Parallel final: public Source
        {
        public:
            void reset() override;

            void getSamples(uint32_t frames, uint16_t channels,
                            uint32_t sampleRate, std::vector<float>& samples) override;
        };

        class Random final: public Source
        {
        public:
            void reset() override;

            void getSamples(uint32_t frames, uint16_t channels,
                            uint32_t sampleRate, std::vector<float>& samples) override;
        };

        class Sequence final: public Source
        {
        public:
            void reset() override;

            void getSamples(uint32_t frames, uint16_t channels,
                            uint32_t sampleRate, std::vector<float>& samples) override;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_CONTAINERS_HPP
