// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SoundData.hpp"
#include "Audio.hpp"
#include "core/Engine.hpp"

namespace ouzel
{
    namespace audio
    {
        SoundData::SoundData()
        {
        }

        SoundData::~SoundData()
        {
        }

        void SoundData::init(const std::string& filename)
        {
            init(engine->getFileSystem()->readFile(filename));
        }

        void SoundData::init(const std::vector<uint8_t>&)
        {
        }

        bool SoundData::getData(Stream* stream, uint32_t frames, uint32_t neededChannels, uint32_t neededSampleRate, float pitch, std::vector<float>& result)
        {
            uint32_t neededFrames = static_cast<uint32_t>(frames * pitch * sampleRate / neededSampleRate);

            if (!readData(stream, neededFrames, tempData))
                return false;

            if (pitch != 1.0F)
            {
                uint32_t srcFrames = static_cast<uint32_t>(tempData.size()) / channels;

                Audio::resample(tempData, srcFrames, resampledData, frames, channels);
            }
            else
                resampledData = tempData;

            if (neededChannels != channels)
            {
                result.resize(frames * neededChannels);

                // front left channel
                if (channels >= 1)
                {
                    uint32_t destination = 0;

                    if (channels >= 1)
                    {
                        uint32_t source = 0;

                        for (uint32_t frame = 0; frame < frames; ++frame)
                        {
                            result[destination] = resampledData[source];
                            destination += neededChannels;
                            source += channels;
                        }
                    }
                    else
                    {
                        // fill the front left channel with zeros
                        for (uint32_t frame = 0; frame < frames; ++frame)
                        {
                            result[destination] = 0.0F;
                            destination += neededChannels;
                        }
                    }
                }

                // front right channel
                if (neededChannels >= 2)
                {
                    uint32_t destination = 0;

                    // sound data has front right channel
                    if (channels >= 2)
                    {
                        uint32_t source = 0;

                        for (uint32_t frame = 0; frame < frames; ++frame)
                        {
                            result[destination + 1] = resampledData[source + 1];
                            destination += neededChannels;
                            source += channels;
                        }
                    }
                    else
                    {
                        // copy the front left channel in to the front right one
                        for (uint32_t frame = 0; frame < frames; ++frame)
                        {
                            result[destination + 1] = result[destination];
                            destination += neededChannels;
                        }
                    }
                }

                // center channel
                if (neededChannels >= 3)
                {
                    uint32_t destination = 0;

                    if (channels >= 3)
                    {
                        uint32_t source = 0;

                        for (uint32_t frame = 0; frame < frames; ++frame)
                        {
                            result[destination + 2] = resampledData[source + 2];
                            destination += neededChannels;
                            source += channels;
                        }
                    }
                    else if (neededChannels >= 2)
                    {
                        // calculate the average of the front left and the front right channel
                        for (uint32_t frame = 0; frame < frames; ++frame)
                        {
                            result[destination + 2] = (result[destination] + result[destination + 1]) / 2.0F;
                            destination += neededChannels;
                        }
                    }
                    else
                    {
                        // copy the front left channel in to the center one
                        for (uint32_t frame = 0; frame < frames; ++frame)
                        {
                            result[destination + 2] = result[destination];
                            destination += neededChannels;
                        }
                    }
                }

                // LFE channel
                if (neededChannels >= 4)
                {
                    uint32_t destination = 0;

                    if (channels >= 4)
                    {
                        uint32_t source = 0;

                        for (uint32_t frame = 0; frame < frames; ++frame)
                        {
                            result[destination + 3] = resampledData[source + 3];
                            destination += neededChannels;
                            source += channels;
                        }
                    }
                    else
                    {
                        // fill the LFE channel with zeros
                        for (uint32_t frame = 0; frame < frames; ++frame)
                        {
                            result[destination + 3] = 0;
                            destination += neededChannels;
                        }
                    }
                }

                // back left channel
                if (neededChannels >= 5)
                {
                    uint32_t destination = 0;

                    // sound data has back left channel
                    if (channels >= 5)
                    {
                        uint32_t source = 0;

                        for (uint32_t frame = 0; frame < frames; ++frame)
                        {
                            result[destination + 4] = resampledData[source + 4];
                            destination += neededChannels;
                            source += channels;
                        }
                    }
                    else
                    {
                        // copy the front left channel in to the back left one
                        for (uint32_t frame = 0; frame < frames; ++frame)
                        {
                            result[destination + 4] = result[destination];
                            destination += neededChannels;
                        }
                    }
                }

                // back right channel
                if (neededChannels >= 6)
                {
                    uint32_t destination = 0;

                    // sound data has back right channel
                    if (channels >= 6)
                    {
                        uint32_t source = 0;

                        for (uint32_t frame = 0; frame < frames; ++frame)
                        {
                            result[destination + 5] = resampledData[source + 5];
                            destination += neededChannels;
                            source += channels;
                        }
                    }
                    else
                    {
                        // copy the front right channel in to the back right one
                        for (uint32_t frame = 0; frame < frames; ++frame)
                        {
                            result[destination + 5] = result[destination + 1];
                            destination += neededChannels;
                        }
                    }
                }
            }
            else
                result = resampledData;

            return true;
        }
    } // namespace audio
} // namespace ouzel
