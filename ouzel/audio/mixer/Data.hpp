// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_MIXER_DATA_HPP
#define OUZEL_AUDIO_MIXER_DATA_HPP

#include <memory>
#include "audio/mixer/Object.hpp"

namespace ouzel
{
    namespace audio
    {
        namespace mixer
        {
            class Data: public Object
            {
            public:
                inline auto getChannels() const noexcept { return channels; }
                inline auto getSampleRate() const noexcept { return sampleRate; }

                void getSamples(uint32_t frames, uint16_t channels, uint32_t sampleRate, std::vector<float>& samples) final
                {
                }

                /*virtual void getData(uint32_t frames,
                                     uint32_t channels,
                                     uint32_t sampleRate,
                                     std::vector<float>& samples) = 0;*/

            protected:
                uint32_t channels = 0;
                uint32_t sampleRate = 0;
            };
        }
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_MIXER_DATA_HPP
