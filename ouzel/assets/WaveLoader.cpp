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
            Loader(initCache, TYPE)
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

                uint32_t offset = 0;

                if (data.size() < 12) // RIFF + size + WAVE
                    throw std::runtime_error("Failed to load sound file, file too small");

                if (data[offset + 0] != 'R' ||
                    data[offset + 1] != 'I' ||
                    data[offset + 2] != 'F' ||
                    data[offset + 3] != 'F')
                    throw std::runtime_error("Failed to load sound file, not a RIFF format");

                offset += 4;

                uint32_t length = static_cast<uint32_t>(data[offset + 0] |
                                                        (data[offset + 1] << 8) |
                                                        (data[offset + 2] << 16) |
                                                        (data[offset + 3] << 24));

                offset += 4;

                if (data.size() < offset + length)
                    throw std::runtime_error("Failed to load sound file, size mismatch");

                if (length < 4 ||
                    data[offset + 0] != 'W' ||
                    data[offset + 1] != 'A' ||
                    data[offset + 2] != 'V' ||
                    data[offset + 3] != 'E')
                    throw std::runtime_error("Failed to load sound file, not a WAVE file");

                offset += 4;

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

                    uint32_t chunkSize = static_cast<uint32_t>(data[offset + 0] |
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

                    offset += ((chunkSize + 1) & 0xFFFFFFFE);
                }

                if (!formatTag)
                    throw std::runtime_error("Failed to load sound file, failed to find a format chunk");

                if (data.empty())
                    throw std::runtime_error("Failed to load sound file, failed to find a data chunk");

                uint32_t bytesPerSample = bitsPerSample / 8;
                uint32_t sampleCount = static_cast<uint32_t>(soundData.size() / bytesPerSample);
                uint32_t frames = sampleCount / channels;
                std::vector<float> samples(sampleCount);

                if (formatTag == PCM)
                {
                    if (bitsPerSample == 8)
                    {
                        for (uint32_t frame = 0; frame < frames; ++frame)
                            for (uint32_t channel = 0; channel < channels; ++channel)
                            {
                                uint32_t s = (frame * channels + channel);
                                samples[channel * frames + frame] = 2.0F * soundData[s] / 255.0F - 1.0F;
                            }
                    }
                    else if (bitsPerSample == 16)
                    {
                        for (uint32_t frame = 0; frame < frames; ++frame)
                            for (uint32_t channel = 0; channel < channels; ++channel)
                            {
                                uint32_t s = (frame * channels + channel) * 2;
                                samples[channel * frames + frame] = static_cast<int16_t>(soundData[s] |
                                                                                         (soundData[s + 1] << 8)) / 32767.0F;
                            }
                    }
                    else if (bitsPerSample == 24)
                    {
                        for (uint32_t frame = 0; frame < frames; ++frame)
                            for (uint32_t channel = 0; channel < channels; ++channel)
                            {
                                uint32_t s = (frame * channels + channel) * 3;
                                samples[channel * frames + frame] = static_cast<float>(static_cast<int32_t>((soundData[s] << 8) |
                                                                                                            (soundData[s + 1] << 16) |
                                                                                                            (soundData[s + 2] << 24)) / 2147483648.0);
                            }
                    }
                    else if (bitsPerSample == 32)
                    {
                        for (uint32_t frame = 0; frame < frames; ++frame)
                            for (uint32_t channel = 0; channel < channels; ++channel)
                            {
                                uint32_t s = (frame * channels + channel) * 4;
                                samples[channel * frames + frame] = static_cast<float>(static_cast<int32_t>(soundData[s] |
                                                                                                            (soundData[s + 1] << 8) |
                                                                                                            (soundData[s + 2] << 16) |
                                                                                                            (soundData[s + 2] << 24)) / 2147483648.0);
                            }
                    }
                    else
                        throw std::runtime_error("Failed to load sound file, unsupported bit depth");
                }
                else if (formatTag == IEEE_FLOAT)
                {
                    if (bitsPerSample == 32)
                    {
                        for (uint32_t frame = 0; frame < frames; ++frame)
                            for (uint32_t channel = 0; channel < channels; ++channel)
                            {
                                uint32_t s = (frame * channels + channel);
                                samples[channel * frames + frame] = reinterpret_cast<float*>(soundData.data())[s];
                            }
                    }
                    else
                        throw std::runtime_error("Failed to load sound file, unsupported bit depth");
                }

                std::shared_ptr<audio::Sound> sound = std::make_shared<audio::PcmClip>(*engine->getAudio(), channels, sampleRate, samples);
                bundle.setSound(name, sound);
            }
            catch (const std::exception&)
            {
                return false;
            }

            return true;
        }
    } // namespace assets
} // namespace ouzel
