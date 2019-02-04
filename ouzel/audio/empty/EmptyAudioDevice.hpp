// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_AUDIODEVICEEMPTY_HPP
#define OUZEL_AUDIO_AUDIODEVICEEMPTY_HPP

#include "audio/AudioDevice.hpp"

namespace ouzel
{
    namespace audio
    {
        class EmptyAudioDevice final: public AudioDevice
        {
        public:
            explicit EmptyAudioDevice(const std::function<void(uint32_t frames, uint16_t channels, uint32_t sampleRate, std::vector<float>& samples)>& initDataGetter);
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_AUDIODEVICEEMPTY_HPP
