// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_MIXER_DATA_HPP
#define OUZEL_AUDIO_MIXER_DATA_HPP

#include <memory>
#include "Object.hpp"

namespace ouzel
{
    namespace audio
    {
        namespace mixer
        {
            class Stream;

            class Data: public Object
            {
            public:
                virtual std::unique_ptr<Stream> createStream() = 0;

                inline auto getChannels() const noexcept { return channels; }
                inline auto getSampleRate() const noexcept { return sampleRate; }

                /*virtual void getData(std::uint32_t frames,
                                     std::uint32_t channels,
                                     std::uint32_t sampleRate,
                                     std::vector<float>& samples) = 0;*/

            protected:
                std::uint32_t channels = 0;
                std::uint32_t sampleRate = 0;
            };
        }
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_MIXER_DATA_HPP
