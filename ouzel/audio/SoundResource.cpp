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

        bool SoundResource::init(const std::shared_ptr<SoundData>& newSoundData)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            soundData = newSoundData;

            if (soundData)
            {
                stream = soundData->createStream();
            }

            dirty |= DIRTY_SOUND_DATA | DIRTY_POSITION | DIRTY_PITCH | DIRTY_GAIN | DIRTY_PLAY_STATE;

            return true;
        }

        void SoundResource::setPosition(const Vector3& newPosition)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            position = newPosition;

            dirty |= DIRTY_POSITION;
        }

        void SoundResource::setPitch(float newPitch)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            pitch = newPitch;

            dirty |= DIRTY_PITCH;
        }

        void SoundResource::setGain(float newGain)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            gain = newGain;

            dirty |= DIRTY_GAIN;
        }

        bool SoundResource::play(bool repeatSound)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            shouldPlay = true;
            repeat = repeatSound;

            dirty |= DIRTY_PLAY_STATE;

            return true;
        }

        bool SoundResource::pause()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            shouldPlay = false;

            dirty |= DIRTY_PLAY_STATE;

            return true;
        }

        bool SoundResource::stop()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            shouldPlay = false;
            reset = true;

            dirty |= DIRTY_PLAY_STATE;

            return true;
        }

        std::vector<uint8_t> SoundResource::getData(uint32_t size, uint16_t channels, uint32_t samplesPerSecond)
        {
            std::vector<uint8_t> result;

            if (!shouldPlay)
            {
                result = std::vector<uint8_t>();
            }
            else if (soundData)
            {
                std::vector<uint8_t> data;

                if (stream)
                {
                    data = soundData->getData(stream.get(), size);
                }
                else
                {
                    data = soundData->getData();
                }

                if (channels == soundData->getChannels() &&
                    samplesPerSecond == soundData->getSamplesPerSecond())
                {
                    result = std::move(data);
                }
                else
                {
                    // channel map
                    // 0 - Front left
                    // 1 - Front right
                    // 2 - Rear left
                    // 3 - Rear right
                    // 4 - Center
                    // 5 - Low-frequency effects

                    result.resize(size);

                    if (channels != soundData->getChannels())
                    {
                        uint32_t dstSamples = size / sizeof(uint16_t);
                        uint32_t srcSamples = static_cast<uint32_t>(data.size()) / sizeof(uint16_t);

                        // left channel
                        if (channels >= 1)
                        {
                            uint16_t* dstBuffer = reinterpret_cast<uint16_t*>(result.data());
                            uint16_t* srcBuffer = reinterpret_cast<uint16_t*>(data.data());

                            for (uint32_t i = 0; i < dstSamples / channels && i < srcSamples / soundData->getChannels(); ++i)
                            {
                                *dstBuffer = *srcBuffer;
                                dstBuffer += channels;
                            }
                        }

                        // right channel
                        if (channels >= 2)
                        {
                            uint16_t* dstBuffer = reinterpret_cast<uint16_t*>(result.data());
                            uint16_t* srcBuffer = reinterpret_cast<uint16_t*>(data.data());

                            // sound data has right channel
                            if (soundData->getChannels() >= 2)
                            {
                                for (uint32_t i = 0; i < dstSamples / channels && i < srcSamples / soundData->getChannels(); ++i)
                                {
                                    *(dstBuffer + 1) = *(srcBuffer + 1);
                                    dstBuffer += channels;
                                }
                            }
                            else
                            {
                                // copy the left channel in to the right one
                                for (uint32_t i = 0; i < dstSamples / channels && i < srcSamples / soundData->getChannels(); ++i)
                                {
                                    *(dstBuffer + 1) = *srcBuffer;
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
                }
            }

            return result;
        }
    } // namespace audio
} // namespace ouzel
