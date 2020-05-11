// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include "EmptyAudioDevice.hpp"

namespace ouzel
{
    namespace audio
    {
        namespace empty
        {
            AudioDevice::AudioDevice(std::uint32_t initBufferSize,
                                     std::uint32_t initSampleRate,
                                     std::uint32_t initChannels,
                                     const std::function<void(std::uint32_t frames,
                                                              std::uint32_t channels,
                                                              std::uint32_t sampleRate,
                                                              std::vector<float>& samples)>& initDataGetter):
                audio::AudioDevice(Driver::empty, initBufferSize, initSampleRate, initChannels, initDataGetter)
            {
            }
        } // namespace empty
    } // namespace audio
} // namespace ouzel
