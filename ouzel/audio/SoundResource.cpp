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
            position = newPosition;
        }

        void SoundResource::setPitch(float newPitch)
        {
            pitch = newPitch;
        }

        void SoundResource::setGain(float newGain)
        {
            gain = newGain;
        }

        bool SoundResource::play(bool repeatSound)
        {
            shouldPlay = true;
            repeat = repeatSound;

            return true;
        }

        bool SoundResource::pause()
        {
            shouldPlay = false;

            return true;
        }

        bool SoundResource::stop()
        {
            shouldPlay = false;
            reset = true;

            return true;
        }

        bool SoundResource::getData(uint32_t samples, uint16_t channels, uint32_t sampleRate, std::vector<float>& result)
        {
            if (!shouldPlay)
            {
                result.resize(0);
            }
            else if (soundData && soundData->getChannels() > 0 && stream)
            {
                if (!soundData->getData(stream.get(), (samples / channels) * soundData->getChannels(), data))
                {
                    return false;
                }

                if (channels != soundData->getChannels())
                {
                    result.resize((data.size() / soundData->getChannels()) * channels);

                    uint32_t srcSamples = static_cast<uint32_t>(data.size());

                    // front left channel
                    if (channels >= 1)
                    {
                        uint32_t destination = 0;

                        if (soundData->getChannels() >= 1)
                        {
                            uint32_t source = 0;

                            for (uint32_t i = 0; i < srcSamples / soundData->getChannels(); ++i)
                            {
                                result[destination] = data[source];
                                destination += channels;
                                source += soundData->getChannels();
                            }
                        }
                        else
                        {
                            // fill the front left channel with zeros
                            for (uint32_t i = 0; i < srcSamples / soundData->getChannels(); ++i)
                            {
                                result[destination] = 0.0f;
                                destination += channels;
                            }
                        }
                    }

                    // front right channel
                    if (channels >= 2)
                    {
                        uint32_t destination = 0;

                        // sound data has front right channel
                        if (soundData->getChannels() >= 2)
                        {
                            uint32_t source = 0;

                            for (uint32_t i = 0; i < srcSamples / soundData->getChannels(); ++i)
                            {
                                result[destination + 1] = data[source + 1];
                                destination += channels;
                                source += soundData->getChannels();
                            }
                        }
                        else
                        {
                            // copy the front left channel in to the front right one
                            for (uint32_t i = 0; i < srcSamples / soundData->getChannels(); ++i)
                            {
                                result[destination + 1] = result[destination];
                                destination += channels;
                            }
                        }
                    }

                    // center channel
                    if (channels >= 3)
                    {
                        uint32_t destination = 0;

                        if (soundData->getChannels() >= 3)
                        {
                            uint32_t source = 0;

                            for (uint32_t i = 0; i < srcSamples / soundData->getChannels(); ++i)
                            {
                                result[destination + 2] = data[source + 2];
                                destination += channels;
                                source += soundData->getChannels();
                            }
                        }
                        else if (channels >= 2)
                        {
                            // calculate the average of the front left and the front right channel
                            for (uint32_t i = 0; i < srcSamples / soundData->getChannels(); ++i)
                            {
                                result[destination + 2] = (result[destination] + result[destination + 1]) / 2.0f;
                                destination += channels;
                            }
                        }
                        else
                        {
                            // copy the front left channel in to the center one
                            for (uint32_t i = 0; i < srcSamples / soundData->getChannels(); ++i)
                            {
                                result[destination + 2] = result[destination];
                                destination += channels;
                            }
                        }
                    }

                    // LFE channel
                    if (channels >= 4)
                    {
                        uint32_t destination = 0;

                        if (soundData->getChannels() >= 4)
                        {
                            uint32_t source = 0;

                            for (uint32_t i = 0; i < srcSamples / soundData->getChannels(); ++i)
                            {
                                result[destination + 3] = data[source + 3];
                                destination += channels;
                                source += soundData->getChannels();
                            }
                        }
                        else
                        {
                            // fill the LFE channel with zeros
                            for (uint32_t i = 0; i < srcSamples / soundData->getChannels(); ++i)
                            {
                                result[destination + 3] = 0;
                                destination += channels;
                            }
                        }
                    }

                    // back left channel
                    if (channels >= 5)
                    {
                        uint32_t destination = 0;

                        // sound data has back left channel
                        if (soundData->getChannels() >= 5)
                        {
                            uint32_t source = 0;

                            for (uint32_t i = 0; i < srcSamples / soundData->getChannels(); ++i)
                            {
                                result[destination + 4] = data[source + 4];
                                destination += channels;
                                source += soundData->getChannels();
                            }
                        }
                        else
                        {
                            // copy the front left channel in to the back left one
                            for (uint32_t i = 0; i < srcSamples / soundData->getChannels(); ++i)
                            {
                                result[destination + 4] = result[destination];
                                destination += channels;
                            }
                        }
                    }

                    // back right channel
                    if (channels >= 6)
                    {
                        uint32_t destination = 0;

                        // sound data has back right channel
                        if (soundData->getChannels() >= 6)
                        {
                            uint32_t source = 0;

                            for (uint32_t i = 0; i < srcSamples / soundData->getChannels(); ++i)
                            {
                                result[destination + 5] = data[source + 5];
                                destination += channels;
                                source += soundData->getChannels();
                            }
                        }
                        else
                        {
                            // copy the front right channel in to the back right one
                            for (uint32_t i = 0; i < srcSamples / soundData->getChannels(); ++i)
                            {
                                result[destination + 5] = result[destination + 1];
                                destination += channels;
                            }
                        }
                    }
                }
                else
                {
                    result = std::move(data);
                }

                uint32_t samplesPerSecond = static_cast<uint32_t>(sampleRate * pitch);

                if (samplesPerSecond != soundData->getSampleRate())
                {
                    // TODO: resample
                }

                std::vector<float> channelVolume(channels, gain);

                for (uint32_t i = 0; i < result.size() / channels; ++i)
                {
                    for (uint32_t channel = 0; channel < channels; ++channel)
                    {
                        result[i * channels + channel] *= channelVolume[channel];
                    }
                }
            }

            return true;
        }
    } // namespace audio
} // namespace ouzel
