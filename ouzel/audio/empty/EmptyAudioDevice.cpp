// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "EmptyAudioDevice.hpp"

namespace ouzel
{
    namespace audio
    {
        EmptyAudioDevice::EmptyAudioDevice(const std::function<void(uint32_t frames, uint16_t channels, uint32_t sampleRate, std::vector<float>& samples)>& initDataGetter):
            AudioDevice(Driver::EMPTY, initDataGetter)
        {
        }
    } // namespace audio
} // namespace ouzel
