// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_MIXER_SOURCE_HPP
#define OUZEL_AUDIO_MIXER_SOURCE_HPP

#include <memory>
#include "audio/mixer/Object.hpp"

namespace ouzel
{
    namespace audio
    {
        namespace mixer
        {
            class Stream;

            class Source: public Object
            {
            public:
                virtual std::unique_ptr<Stream> createStream() = 0;

                uint16_t getChannels() const { return channels; }
                uint32_t getSampleRate() const { return sampleRate; }

                /*virtual void getData(uint32_t frames,
                                     uint16_t channels,
                                     uint32_t sampleRate,
                                     std::vector<float>& samples) = 0;*/

            protected:
                uint16_t channels = 0;
                uint32_t sampleRate = 0;
            };
        }
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_MIXER_SOURCE_HPP
