// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

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
                AudioDevice(std::uint32_t initBufferSize,
                            std::uint32_t initSampleRate,
                            std::uint32_t initChannels,
                            const std::function<void(std::uint32_t frames,
                                                     std::uint32_t channels,
                                                     std::uint32_t sampleRate,
                                                     std::vector<float>& samples)>& initDataGetter);

                void start() final {}
                void stop() final {}
            };
        } // namespace empty
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_EMPTYAUDIODEVICE_HPP
