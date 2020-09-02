// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include "AudioDevice.hpp"

namespace ouzel::audio
{
    AudioDevice::AudioDevice(Driver initDriver,
                             const Settings& settings,
                             const std::function<void(std::uint32_t frames,
                                                      std::uint32_t channels,
                                                      std::uint32_t sampleRate,
                                                      std::vector<float>& samples)>& initDataGetter):
        driver(initDriver),
        bufferSize(settings.bufferSize),
        sampleRate(settings.sampleRate),
        channels((settings.channels != 0) ? settings.channels : 2), // TODO: leave it zero and let subclasses decide the number of channels
        dataGetter(initDataGetter)
    {
    }

    void AudioDevice::getData(std::uint32_t frames, std::vector<std::uint8_t>& result)
    {
        dataGetter(frames, channels, sampleRate, buffer);

        switch (sampleFormat)
        {
            case SampleFormat::signedInt16:
            {
                result.resize(frames * channels * sizeof(std::int16_t));
                auto resultPtr = reinterpret_cast<std::int16_t*>(result.data());

                for (std::uint32_t channel = 0; channel < channels; ++channel)
                {
                    float* bufferChannel = &buffer[channel * frames];

                    for (std::uint32_t frame = 0; frame < frames; ++frame)
                        resultPtr[frame * channels + channel] = static_cast<std::int16_t>(bufferChannel[frame] * 32767.0F);
                }
                break;
            }
            case SampleFormat::float32:
            {
                result.resize(frames * channels * sizeof(float));
                auto resultPtr = reinterpret_cast<float*>(result.data());

                for (std::uint32_t channel = 0; channel < channels; ++channel)
                {
                    float* bufferChannel = &buffer[channel * frames];

                    for (std::uint32_t frame = 0; frame < frames; ++frame)
                        resultPtr[frame * channels + channel] = bufferChannel[frame];
                }
                break;
            }
            default:
                throw std::runtime_error("Invalid sample format");
        }
    }
}
