// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_EMPTYAUDIODEVICE_HPP
#define OUZEL_AUDIO_EMPTYAUDIODEVICE_HPP

#include "audio/AudioDevice.hpp"

namespace ouzel
{
    namespace audio
    {
        namespace empty
        {
            class AudioDevice final: public audio::AudioDevice
            {
            public:
                AudioDevice(uint32_t initBufferSize,
                            uint32_t initSampleRate,
                            uint32_t initChannels,
                            const std::function<void(uint32_t frames,
                                                     uint32_t channels,
                                                     uint32_t sampleRate,
                                                     std::vector<float>& samples)>& initDataGetter);

                void start() final {}
                void stop() final {}
            };
        } // namespace empty
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_EMPTYAUDIODEVICE_HPP
