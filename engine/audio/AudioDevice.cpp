// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include "AudioDevice.hpp"

namespace ouzel
{
    namespace audio
    {
        AudioDevice::AudioDevice(Driver initDriver,
                                 std::uint32_t initBufferSize,
                                 std::uint32_t initSampleRate,
                                 std::uint32_t initChannels,
                                 const std::function<void(std::uint32_t frames,
                                                          std::uint32_t channels,
                                                          std::uint32_t sampleRate,
                                                          std::vector<float>& samples)>& initDataGetter):
            driver(initDriver),
            bufferSize(initBufferSize),
            sampleRate(initSampleRate),
            channels((initChannels != 0) ? initChannels : 2), // TODO: leave it zero and let subclasses decide the number of channels
            dataGetter(initDataGetter)
        {
        }

        void AudioDevice::getData(std::uint32_t frames, std::vector<std::uint8_t>& result)
        {
            dataGetter(frames, channels, sampleRate, buffer);

            switch (sampleFormat)
            {
                case SampleFormat::SignedInt16:
                {
                    result.resize(frames * channels * sizeof(std::int16_t));
                    std::int16_t* resultPtr = reinterpret_cast<std::int16_t*>(result.data());

                    for (std::uint32_t channel = 0; channel < channels; ++channel)
                    {
                        float* bufferChannel = &buffer[channel * frames];

                        for (std::uint32_t frame = 0; frame < frames; ++frame)
                            resultPtr[frame * channels + channel] = static_cast<std::int16_t>(bufferChannel[frame] * 32767.0F);
                    }
                    break;
                }
                case SampleFormat::Float32:
                {
                    result.resize(frames * channels * sizeof(float));
                    float* resultPtr = reinterpret_cast<float*>(result.data());

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
    } // namespace audio
} // namespace ouzel
