// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "AudioDevice.hpp"

namespace ouzel
{
    namespace audio
    {
        AudioDevice::AudioDevice(Driver initDriver,
                                 uint32_t initBufferSize,
                                 uint32_t initSampleRate,
                                 uint16_t initChannels,
                                 const std::function<void(uint32_t frames,
                                                          uint16_t channels,
                                                          uint32_t sampleRate,
                                                          std::vector<float>& samples)>& initDataGetter):
            driver(initDriver),
            bufferSize(initBufferSize),
            sampleRate(initSampleRate),
            channels((initChannels != 0) ? initChannels : 2), // TODO: leave it zero and let subclasses decide the number of channels
            dataGetter(initDataGetter)
        {
        }

        AudioDevice::~AudioDevice()
        {
        }

        void AudioDevice::getData(uint32_t frames, std::vector<uint8_t>& result)
        {
            dataGetter(frames, channels, sampleRate, buffer);

            switch (sampleFormat)
            {
                case SampleFormat::SInt16:
                {
                    result.resize(frames * channels * sizeof(int16_t));
                    int16_t* resultPtr = reinterpret_cast<int16_t*>(result.data());

                    for (uint16_t channel = 0; channel < channels; ++channel)
                    {
                        float* bufferChannel = &buffer[channel * frames];

                        for (uint32_t frame = 0; frame < frames; ++frame)
                            resultPtr[frame * channels + channel] = static_cast<int16_t>(bufferChannel[frame] * 32767.0F);
                    }
                    break;
                }
                case SampleFormat::Float32:
                {
                    result.resize(frames * channels * sizeof(float));
                    float* resultPtr = reinterpret_cast<float*>(result.data());

                    for (uint16_t channel = 0; channel < channels; ++channel)
                    {
                        float* bufferChannel = &buffer[channel * frames];

                        for (uint32_t frame = 0; frame < frames; ++frame)
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
