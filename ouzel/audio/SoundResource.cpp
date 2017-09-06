// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cmath>
#include "SoundResource.hpp"
#include "AudioDevice.hpp"
#include "SoundData.hpp"
#include "Stream.hpp"
#include "math/MathUtils.hpp"

static void resampleLerp(const std::vector<float>& src, uint32_t srcFrames,
                         std::vector<float>& dst, uint32_t dstFrames,
                         uint32_t channels)
{
    if (dstFrames > 0) // do resampling only if setination is not empty
    {
        if (srcFrames == 0) // source is empty
        {
            dst.resize(dstFrames * channels);

            std::fill(dst.begin(), dst.end(), 0.0f);
        }
        else
        {
            float srcIncrement = static_cast<float>(srcFrames - 1) / static_cast<float>(dstFrames - 1);
            float srcPosition = 0.0f;

            dst.resize(dstFrames * channels);

            for (uint32_t frame = 0; frame < dstFrames - 1; ++frame)
            {
                uint32_t srcCurrentFrame = static_cast<uint32_t>(srcPosition);
                float frac = srcPosition - srcCurrentFrame;

                srcPosition += srcIncrement;
                uint32_t srcNextFrame = srcCurrentFrame + 1;

                for (uint32_t channel = 0; channel < channels; ++channel)
                {
                    uint32_t srcCurrentPosition = srcCurrentFrame * channels + channel;
                    uint32_t srcNextPosition = srcNextFrame * channels + channel;

                    dst[frame * channels + channel] = ouzel::lerp(src[srcCurrentPosition], src[srcNextPosition], frac);
                }
            }

            // fill the last frame of the destination with the last frame of the source
            for (uint32_t channel = 0; channel < channels; ++channel)
            {
                dst[(dstFrames - 1) * channels + channel] = src[(srcFrames - 1) * channels + channel];
            }
        }
    }
}

namespace ouzel
{
    namespace audio
    {
        SoundResource::SoundResource(AudioDevice* aAudioDevice):
            audioDevice(aAudioDevice)
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

        void SoundResource::setRolloffFactor(float newRolloffFactor)
        {
            rolloffFactor = newRolloffFactor;
        }

        void SoundResource::setMinDistance(float newMinDistance)
        {
            minDistance = newMinDistance;
        }

        void SoundResource::setMaxDistance(float newMaxDistance)
        {
            maxDistance = newMaxDistance;
        }

        bool SoundResource::play(bool repeatSound)
        {
            playing = true;
            repeat = repeatSound;

            return true;
        }

        bool SoundResource::pause()
        {
            playing = false;

            return true;
        }

        bool SoundResource::stop()
        {
            playing = false;
            reset = true;

            return true;
        }

        bool SoundResource::getData(uint32_t frames, uint16_t channels, uint32_t sampleRate, std::vector<float>& result)
        {
            if (!playing)
            {
                result.clear();
            }
            else if (soundData && soundData->getChannels() > 0 && stream)
            {
                float finalPitch = pitch * soundData->getSampleRate() / sampleRate;
                uint32_t neededFrames = static_cast<uint32_t>(frames * finalPitch);

                uint32_t resetCount;
                if (!soundData->getData(stream.get(), neededFrames, repeat, resetCount, data))
                {
                    return false;
                }

                if (!repeat && resetCount > 0) playing = false;

                if (finalPitch != 1.0f)
                {
                    uint32_t srcFrames = static_cast<uint32_t>(data.size()) / soundData->getChannels();
                    uint32_t dstFrames = static_cast<uint32_t>(frames * (static_cast<float>(srcFrames) / neededFrames));

                    resampleLerp(data, srcFrames, resampledData, dstFrames, soundData->getChannels());
                }
                else
                {
                    resampledData = data;
                }

                if (channels != soundData->getChannels())
                {
                    uint32_t srcFrames = static_cast<uint32_t>(resampledData.size()) / soundData->getChannels();

                    result.resize(srcFrames * channels);

                    // front left channel
                    if (channels >= 1)
                    {
                        uint32_t destination = 0;

                        if (soundData->getChannels() >= 1)
                        {
                            uint32_t source = 0;

                            for (uint32_t frame = 0; frame < srcFrames; ++frame)
                            {
                                result[destination] = resampledData[source];
                                destination += channels;
                                source += soundData->getChannels();
                            }
                        }
                        else
                        {
                            // fill the front left channel with zeros
                            for (uint32_t frame = 0; frame < srcFrames; ++frame)
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

                            for (uint32_t frame = 0; frame < srcFrames; ++frame)
                            {
                                result[destination + 1] = resampledData[source + 1];
                                destination += channels;
                                source += soundData->getChannels();
                            }
                        }
                        else
                        {
                            // copy the front left channel in to the front right one
                            for (uint32_t frame = 0; frame < srcFrames; ++frame)
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

                            for (uint32_t frame = 0; frame < srcFrames; ++frame)
                            {
                                result[destination + 2] = resampledData[source + 2];
                                destination += channels;
                                source += soundData->getChannels();
                            }
                        }
                        else if (channels >= 2)
                        {
                            // calculate the average of the front left and the front right channel
                            for (uint32_t frame = 0; frame < srcFrames; ++frame)
                            {
                                result[destination + 2] = (result[destination] + result[destination + 1]) / 2.0f;
                                destination += channels;
                            }
                        }
                        else
                        {
                            // copy the front left channel in to the center one
                            for (uint32_t frame = 0; frame < srcFrames; ++frame)
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

                            for (uint32_t frame = 0; frame < srcFrames; ++frame)
                            {
                                result[destination + 3] = resampledData[source + 3];
                                destination += channels;
                                source += soundData->getChannels();
                            }
                        }
                        else
                        {
                            // fill the LFE channel with zeros
                            for (uint32_t frame = 0; frame < srcFrames; ++frame)
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

                            for (uint32_t frame = 0; frame < srcFrames; ++frame)
                            {
                                result[destination + 4] = resampledData[source + 4];
                                destination += channels;
                                source += soundData->getChannels();
                            }
                        }
                        else
                        {
                            // copy the front left channel in to the back left one
                            for (uint32_t frame = 0; frame < srcFrames; ++frame)
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

                            for (uint32_t frame = 0; frame < srcFrames; ++frame)
                            {
                                result[destination + 5] = resampledData[source + 5];
                                destination += channels;
                                source += soundData->getChannels();
                            }
                        }
                        else
                        {
                            // copy the front right channel in to the back right one
                            for (uint32_t frame = 0; frame < srcFrames; ++frame)
                            {
                                result[destination + 5] = result[destination + 1];
                                destination += channels;
                            }
                        }
                    }
                }
                else
                {
                    result = resampledData;
                }

                Vector3 offset; // = relativePosition ? position : position - audioDevice->getListenerPosition();
                float distance = clamp(offset.length(), minDistance, maxDistance);
                float attenuation = minDistance / (minDistance + rolloffFactor * (distance - minDistance)); // inverse distance

                std::vector<float> channelVolume(channels, gain * attenuation);

                if (channelVolume.size() > 1)
                {
                    Quaternion inverseRotation;// = -audioDevice->getListenerRotation();
                    Vector3 relative = inverseRotation * offset;
                    relative.normalize();
                    float angle = atan2f(relative.x, relative.z);

                    // constant power panning
                    channelVolume[0] = SQRT2 / 2.0f * (cosf(angle) - sinf(angle));
                    channelVolume[1] = SQRT2 / 2.0f * (cosf(angle) + sinf(angle));
                }

                for (uint32_t frame = 0; frame < result.size() / channels; ++frame)
                {
                    for (uint32_t channel = 0; channel < channels; ++channel)
                    {
                        result[frame * channels + channel] *= channelVolume[channel];
                    }
                }
            }

            return true;
        }
    } // namespace audio
} // namespace ouzel
