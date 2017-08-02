// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SoundResource.h"
#include "SoundData.h"
#include "Stream.h"

namespace ouzel
{
    namespace audio
    {
        SoundResource::SoundResource()
        {
        }

        SoundResource::~SoundResource()
        {
        }

        bool SoundResource::init(const std::shared_ptr<SoundData>& newSoundData, bool newRelativePosition)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            soundData = newSoundData;
            relativePosition = newRelativePosition;

            if (soundData)
            {
                stream = soundData->createStream();
            }

            return true;
        }

        void SoundResource::setPosition(const Vector3& newPosition)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            position = newPosition;
        }

        void SoundResource::setPitch(float newPitch)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            pitch = newPitch;
        }

        void SoundResource::setGain(float newGain)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            gain = newGain;
        }

        bool SoundResource::play(bool repeatSound)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            shouldPlay = true;
            repeat = repeatSound;

            return true;
        }

        bool SoundResource::pause()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            shouldPlay = false;

            return true;
        }

        bool SoundResource::stop()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            shouldPlay = false;
            reset = true;

            return true;
        }

        bool SoundResource::getData(uint32_t samples, uint16_t channels, uint32_t samplesPerSecond, std::vector<float>& result)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            if (!shouldPlay)
            {
                result.resize(0);
            }
            else if (soundData && soundData->getChannels() > 0 && stream)
            {
                std::vector<float> data;
                if (!soundData->getData(stream.get(), (samples / channels) * soundData->getChannels(), data))
                {
                    return false;
                }

                if (channels == soundData->getChannels() &&
                    samplesPerSecond == soundData->getSamplesPerSecond())
                {
                    result = std::move(data);
                }
                else
                {
                    result.resize(samples);

                    if (channels != soundData->getChannels())
                    {
                        uint32_t dstSamples = samples;
                        uint32_t srcSamples = static_cast<uint32_t>(data.size());

                        // front left channel
                        if (channels >= 1)
                        {
                            int16_t* dstBuffer = reinterpret_cast<int16_t*>(result.data());

                            if (soundData->getChannels() >= 1)
                            {
                                int16_t* srcBuffer = reinterpret_cast<int16_t*>(data.data());

                                for (uint32_t i = 0; i < dstSamples / channels && i < srcSamples / soundData->getChannels(); ++i)
                                {
                                    *dstBuffer = *srcBuffer;
                                    dstBuffer += channels;
                                    srcBuffer += soundData->getChannels();
                                }
                            }
                            else
                            {
                                // fill the front left channel with zeros
                                for (uint32_t i = 0; i < dstSamples / channels && i < srcSamples / soundData->getChannels(); ++i)
                                {
                                    *dstBuffer = 0;
                                    dstBuffer += channels;
                                }
                            }
                        }

                        // front right channel
                        if (channels >= 2)
                        {
                            int16_t* dstBuffer = reinterpret_cast<int16_t*>(result.data());

                            // sound data has front right channel
                            if (soundData->getChannels() >= 2)
                            {
                                int16_t* srcBuffer = reinterpret_cast<int16_t*>(data.data());

                                for (uint32_t i = 0; i < dstSamples / channels && i < srcSamples / soundData->getChannels(); ++i)
                                {
                                    *(dstBuffer + 1) = *(srcBuffer + 1);
                                    dstBuffer += channels;
                                    srcBuffer += soundData->getChannels();
                                }
                            }
                            else
                            {
                                // copy the front left channel in to the front right one
                                for (uint32_t i = 0; i < dstSamples / channels && i < srcSamples / soundData->getChannels(); ++i)
                                {
                                    *(dstBuffer + 1) = *dstBuffer;
                                    dstBuffer += channels;
                                }
                            }
                        }

                        // center channel
                        if (channels >= 3)
                        {
                            int16_t* dstBuffer = reinterpret_cast<int16_t*>(result.data());

                            if (soundData->getChannels() >= 3)
                            {
                                int16_t* srcBuffer = reinterpret_cast<int16_t*>(data.data());

                                for (uint32_t i = 0; i < dstSamples / channels && i < srcSamples / soundData->getChannels(); ++i)
                                {
                                    *(dstBuffer + 2) = *(srcBuffer + 2);
                                    dstBuffer += channels;
                                    srcBuffer += soundData->getChannels();
                                }
                            }
                            else if (channels >= 2)
                            {
                                // calculate the average of the front left and the front right channel
                                for (uint32_t i = 0; i < dstSamples / channels && i < srcSamples / soundData->getChannels(); ++i)
                                {
                                    *(dstBuffer + 2) = (*dstBuffer + *(dstBuffer + 1)) / 2;
                                    dstBuffer += channels;
                                }
                            }
                            else
                            {
                                // copy the front left channel in to the center one
                                for (uint32_t i = 0; i < dstSamples / channels && i < srcSamples / soundData->getChannels(); ++i)
                                {
                                    *(dstBuffer + 2) = *dstBuffer;
                                    dstBuffer += channels;
                                }
                            }
                        }

                        // LFE channel
                        if (channels >= 4)
                        {
                            int16_t* dstBuffer = reinterpret_cast<int16_t*>(result.data());

                            if (soundData->getChannels() >= 4)
                            {
                                int16_t* srcBuffer = reinterpret_cast<int16_t*>(data.data());

                                for (uint32_t i = 0; i < dstSamples / channels && i < srcSamples / soundData->getChannels(); ++i)
                                {
                                    *(dstBuffer + 3) = *(srcBuffer + 3);
                                    dstBuffer += channels;
                                    srcBuffer += soundData->getChannels();
                                }
                            }
                            else
                            {
                                // fill the LFE channel with zeros
                                for (uint32_t i = 0; i < dstSamples / channels && i < srcSamples / soundData->getChannels(); ++i)
                                {
                                    *(dstBuffer + 3) = 0;
                                    dstBuffer += channels;
                                }
                            }
                        }

                        // back left channel
                        if (channels >= 5)
                        {
                            int16_t* dstBuffer = reinterpret_cast<int16_t*>(result.data());

                            // sound data has back left channel
                            if (soundData->getChannels() >= 5)
                            {
                                int16_t* srcBuffer = reinterpret_cast<int16_t*>(data.data());

                                for (uint32_t i = 0; i < dstSamples / channels && i < srcSamples / soundData->getChannels(); ++i)
                                {
                                    *(dstBuffer + 4) = *(srcBuffer + 4);
                                    dstBuffer += channels;
                                    srcBuffer += soundData->getChannels();
                                }
                            }
                            else
                            {
                                // copy the front left channel in to the back left one
                                for (uint32_t i = 0; i < dstSamples / channels && i < srcSamples / soundData->getChannels(); ++i)
                                {
                                    *(dstBuffer + 4) = *dstBuffer;
                                    dstBuffer += channels;
                                }
                            }
                        }

                        // back right channel
                        if (channels >= 6)
                        {
                            int16_t* dstBuffer = reinterpret_cast<int16_t*>(result.data());

                            // sound data has back right channel
                            if (soundData->getChannels() >= 6)
                            {
                                int16_t* srcBuffer = reinterpret_cast<int16_t*>(data.data());

                                for (uint32_t i = 0; i < dstSamples / channels && i < srcSamples / soundData->getChannels(); ++i)
                                {
                                    *(dstBuffer + 5) = *(srcBuffer + 5);
                                    dstBuffer += channels;
                                    srcBuffer += soundData->getChannels();
                                }
                            }
                            else
                            {
                                // copy the front right channel in to the back right one
                                for (uint32_t i = 0; i < dstSamples / channels && i < srcSamples / soundData->getChannels(); ++i)
                                {
                                    *(dstBuffer + 5) = *(dstBuffer + 1);
                                    dstBuffer += channels;
                                }
                            }
                        }
                    }
                    else
                    {
                        result = std::move(data);
                    }

                    if (samplesPerSecond != soundData->getSamplesPerSecond())
                    {
                        // TODO: resample
                    }

                    std::vector<float> channelVolume(channels, gain);

                    uint32_t samples = static_cast<uint32_t>(result.size()) / sizeof(int16_t);
                    int16_t* buffer = reinterpret_cast<int16_t*>(result.data());

                    for (uint32_t i = 0; i < samples / channels; ++i)
                    {
                        for (uint32_t channel = 0; channel < channels; ++channel)
                        {
                            *buffer = static_cast<int16_t>(*buffer * channelVolume[channel]);

                            ++buffer;
                        }
                    }
                }
            }

            return true;
        }
    } // namespace audio
} // namespace ouzel
