// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

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
            EmptyAudioDevice(uint32_t initBufferSize,
                             uint32_t initSampleRate,
                             uint16_t initChannels,
                             const std::function<void(uint32_t frames,
                                                      uint16_t channels,
                                                      uint32_t sampleRate,
                                                      std::vector<float>& samples)>& initDataGetter);

            void start() final {}
            void stop() final {}
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_AUDIODEVICEEMPTY_HPP
