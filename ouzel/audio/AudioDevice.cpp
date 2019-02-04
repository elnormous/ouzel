// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include "AudioDevice.hpp"

namespace ouzel
{
    namespace audio
    {
        AudioDevice::AudioDevice(Driver initDriver,
                                 const std::function<void(uint32_t frames, uint16_t channels, uint32_t sampleRate, std::vector<float>& samples)>& initDataGetter):
            driver(initDriver), dataGetter(initDataGetter)
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
                case SampleFormat::SINT16:
                {
                    result.resize(frames * channels * sizeof(int16_t));
                    int16_t* resultPtr = reinterpret_cast<int16_t*>(result.data());

                    for (uint32_t i = 0; i < buffer.size(); ++i)
                    {
                        *resultPtr = static_cast<int16_t>(buffer[i] * 32767.0F);
                        ++resultPtr;
                    }
                    break;
                }
                case SampleFormat::FLOAT32:
                {
                    result.reserve(frames * channels * sizeof(float));
                    result.assign(reinterpret_cast<uint8_t*>(buffer.data()),
                                  reinterpret_cast<uint8_t*>(buffer.data()) + buffer.size() * sizeof(float));
                    break;
                }
                default:
                    throw std::runtime_error("Invalid sample format");
            }
        }
    } // namespace audio
} // namespace ouzel
