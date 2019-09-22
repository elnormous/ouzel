// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "WaveLoader.hpp"
#include "Bundle.hpp"
#include "audio/PcmClip.hpp"
#include "core/Engine.hpp"

enum WaveFormat
{
    PCM = 1,
    IEEE_FLOAT = 3
};

namespace ouzel
{
    namespace assets
    {
        WaveLoader::WaveLoader(Cache& initCache):
            Loader(initCache, Loader::Sound)
        {
        }

        bool WaveLoader::loadAsset(Bundle& bundle,
                                   const std::string& name,
                                   const std::vector<uint8_t>& data,
                                   bool)
        {
            try
            {
                uint16_t channels = 0;
                uint32_t sampleRate = 0;

                const uint32_t formatOffset = 0;

                if (data.size() < 12) // RIFF + size + WAVE
                    throw std::runtime_error("Failed to load sound file, file too small");

                if (data[formatOffset + 0] != 'R' ||
                    data[formatOffset + 1] != 'I' ||
                    data[formatOffset + 2] != 'F' ||
                    data[formatOffset + 3] != 'F')
                    throw std::runtime_error("Failed to load sound file, not a RIFF format");

                const uint32_t lengthOffset = formatOffset + 4;

                auto length = static_cast<uint32_t>(data[lengthOffset + 0] |
                                                    (data[lengthOffset + 1] << 8) |
                                                    (data[lengthOffset + 2] << 16) |
                                                    (data[lengthOffset + 3] << 24));

                const uint32_t typeOffset = lengthOffset + 4;

                if (data.size() < typeOffset + length)
                    throw std::runtime_error("Failed to load sound file, size mismatch");

                if (length < 4 ||
                    data[typeOffset + 0] != 'W' ||
                    data[typeOffset + 1] != 'A' ||
                    data[typeOffset + 2] != 'V' ||
                    data[typeOffset + 3] != 'E')
                    throw std::runtime_error("Failed to load sound file, not a WAVE file");

                uint32_t offset = typeOffset + 4;

                uint16_t bitsPerSample = 0;
                uint16_t formatTag = 0;
                std::vector<uint8_t> soundData;

                while (offset < data.size())
                {
                    if (data.size() < offset + 8)
                        throw std::runtime_error("Failed to load sound file, not enough data to read chunk");

                    uint8_t chunkHeader[4];
                    chunkHeader[0] = data[offset + 0];
                    chunkHeader[1] = data[offset + 1];
                    chunkHeader[2] = data[offset + 2];
                    chunkHeader[3] = data[offset + 3];

                    offset += 4;

                    auto chunkSize = static_cast<uint32_t>(data[offset + 0] |
                                                           (data[offset + 1] << 8) |
                                                           (data[offset + 2] << 16) |
                                                           (data[offset + 3] << 24));
                    offset += 4;

                    if (data.size() < offset + chunkSize)
                        throw std::runtime_error("Failed to load sound file, not enough data to read chunk");

                    if (chunkHeader[0] == 'f' && chunkHeader[1] == 'm' && chunkHeader[2] == 't' && chunkHeader[3] == ' ')
                    {
                        if (chunkSize < 16)
                            throw std::runtime_error("Failed to load sound file, not enough data to read chunk");

                        uint32_t i = offset;

                        formatTag = static_cast<uint16_t>(data[i + 0] |
                                                          (data[i + 1] << 8));
                        i += 2;

                        if (formatTag != PCM && formatTag != IEEE_FLOAT)
                            throw std::runtime_error("Failed to load sound file, unsupported format");

                        channels = static_cast<uint16_t>(data[i + 0] |
                                                         (data[i + 1] << 8));
                        i += 2;

                        if (!channels)
                            throw std::runtime_error("Failed to load sound file, invalid channel count");

                        sampleRate = static_cast<uint32_t>(data[i + 0] |
                                                           (data[i + 1] << 8) |
                                                           (data[i + 2] << 16) |
                                                           (data[i + 3] << 24));
                        i += 4;

                        if (!sampleRate)
                            throw std::runtime_error("Failed to load sound file, invalid sample rate");

                        i += 4; // average bytes per second

                        i += 2; // block align

                        bitsPerSample = static_cast<uint16_t>(data[i + 0] |
                                                              (data[i + 1] << 8));
                        i += 2;

                        if (bitsPerSample != 8 && bitsPerSample != 16 &&
                            bitsPerSample != 24 && bitsPerSample != 32)
                            throw std::runtime_error("Failed to load sound file, unsupported bit depth");
                    }
                    else if (chunkHeader[0] == 'd' && chunkHeader[1] == 'a' && chunkHeader[2] == 't' && chunkHeader[3] == 'a')
                        soundData.assign(data.begin() + static_cast<int>(offset), data.begin() + static_cast<int>(offset + chunkSize));

                    // padding
                    offset += ((chunkSize + 1) & 0xFFFFFFFE);
                }

                if (!formatTag)
                    throw std::runtime_error("Failed to load sound file, failed to find a format chunk");

                if (data.empty())
                    throw std::runtime_error("Failed to load sound file, failed to find a data chunk");

                auto sampleCount = static_cast<uint32_t>(soundData.size() / (bitsPerSample / 8));
                auto frames = sampleCount / channels;
                std::vector<float> samples(sampleCount);

                if (formatTag == PCM)
                {
                    switch (bitsPerSample)
                    {
                        case 8:
                        {
                            for (uint32_t channel = 0; channel < channels; ++channel)
                            {
                                float* outputChannel = &samples[channel * frames];

                                for (uint32_t frame = 0; frame < frames; ++frame)
                                {
                                    uint8_t* sourceData = &soundData[frame * channels + channel];
                                    outputChannel[frame] = 2.0F * sourceData[0] / 255.0F - 1.0F;
                                }
                            }
                            break;
                        }
                        case 16:
                        {
                            for (uint32_t channel = 0; channel < channels; ++channel)
                            {
                                float* outputChannel = &samples[channel * frames];

                                for (uint32_t frame = 0; frame < frames; ++frame)
                                {
                                    uint8_t* sourceData = &soundData[(frame * channels + channel) * 2];
                                    outputChannel[frame] = static_cast<int16_t>(sourceData[0] |
                                                                                (sourceData[1] << 8)) / 32767.0F;
                                }
                            }
                            break;
                        }
                        case 24:
                        {
                            for (uint32_t channel = 0; channel < channels; ++channel)
                            {
                                float* outputChannel = &samples[channel * frames];

                                for (uint32_t frame = 0; frame < frames; ++frame)
                                {
                                    uint8_t* sourceData = &soundData[(frame * channels + channel) * 3];
                                    outputChannel[frame] = static_cast<float>(static_cast<int32_t>((sourceData[0] << 8) |
                                                                                                   (sourceData[1] << 16) |
                                                                                                   (sourceData[2] << 24)) / 2147483648.0);
                                }
                            }
                            break;
                        }
                        case 32:
                        {
                            for (uint32_t channel = 0; channel < channels; ++channel)
                            {
                                float* outputChannel = &samples[channel * frames];

                                for (uint32_t frame = 0; frame < frames; ++frame)
                                {
                                    uint8_t* sourceData = &soundData[(frame * channels + channel) * 4];
                                    outputChannel[frame] = static_cast<float>(static_cast<int32_t>(sourceData[0] |
                                                                                                   (sourceData[1] << 8) |
                                                                                                   (sourceData[2] << 16) |
                                                                                                   (sourceData[3] << 24)) / 2147483648.0);
                                }
                            }
                            break;
                        }
                        default:
                            throw std::runtime_error("Failed to load sound file, unsupported bit depth");
                    }
                }
                else if (formatTag == IEEE_FLOAT)
                {
                    if (bitsPerSample == 32)
                    {
                        for (uint32_t channel = 0; channel < channels; ++channel)
                        {
                            float* outputChannel = &samples[channel * frames];

                            for (uint32_t frame = 0; frame < frames; ++frame)
                            {
                                uint8_t* sourceData = &soundData[(frame * channels + channel) * 4];
                                memcpy(&outputChannel[frame], sourceData, sizeof(float));
                            }
                        }
                    }
                    else
                        throw std::runtime_error("Failed to load sound file, unsupported bit depth");
                }

                auto sound = std::make_unique<audio::PcmClip>(*engine->getAudio(), channels, sampleRate, samples);
                bundle.setSound(name, std::move(sound));
            }
            catch (const std::exception&)
            {
                return false;
            }

            return true;
        }
    } // namespace assets
} // namespace ouzel
