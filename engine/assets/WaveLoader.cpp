// Ouzel by Elviss Strazdins

#include <cstring>
#include "WaveLoader.hpp"
#include "Bundle.hpp"
#include "../audio/PcmClip.hpp"
#include "../core/Engine.hpp"

namespace
{
    constexpr std::uint16_t WAVE_FORMAT_PCM = 1;
    constexpr std::uint16_t WAVE_FORMAT_IEEE_FLOAT = 3;

    constexpr std::int32_t convert24bitTo232bit(std::byte data[3]) noexcept
    {
        return (static_cast<std::uint8_t>(data[2]) & 0x80) ?
            static_cast<int32_t>(static_cast<std::uint8_t>(data[0]) |
                                 (static_cast<std::uint8_t>(data[1]) << 8) |
                                 (static_cast<std::uint8_t>(data[2]) << 16) |
                                 (static_cast<std::uint8_t>(0xFF) << 24)) :
            static_cast<int32_t>(static_cast<std::uint8_t>(data[0]) |
                                 (static_cast<std::uint8_t>(data[1]) << 8) |
                                 (static_cast<std::uint8_t>(data[2]) << 16));
    }
}

namespace ouzel::assets
{
    WaveLoader::WaveLoader():
        Loader{Type::sound}
    {
    }

    bool WaveLoader::loadAsset(Cache& cache,
                               Bundle& bundle,
                               const std::string& name,
                               const std::vector<std::byte>& data,
                               bool)
    {
        (void)cache;

        try
        {
            std::uint32_t channels = 0U;
            std::uint32_t sampleRate = 0U;

            const std::size_t formatOffset = 0U;

            if (data.size() < 12) // RIFF + size + WAVE
                throw std::runtime_error("Failed to load sound file, file too small");

            if (static_cast<char>(data[formatOffset + 0]) != 'R' ||
                static_cast<char>(data[formatOffset + 1]) != 'I' ||
                static_cast<char>(data[formatOffset + 2]) != 'F' ||
                static_cast<char>(data[formatOffset + 3]) != 'F')
                throw std::runtime_error("Failed to load sound file, not a RIFF format");

            const std::size_t lengthOffset = formatOffset + 4;

            const auto length = static_cast<std::uint32_t>(data[lengthOffset + 0]) |
                (static_cast<std::uint32_t>(data[lengthOffset + 1]) << 8) |
                (static_cast<std::uint32_t>(data[lengthOffset + 2]) << 16) |
                (static_cast<std::uint32_t>(data[lengthOffset + 3]) << 24);

            const std::size_t typeOffset = lengthOffset + 4;

            if (data.size() < typeOffset + length)
                throw std::runtime_error("Failed to load sound file, size mismatch");

            if (length < 4 ||
                static_cast<char>(data[typeOffset + 0]) != 'W' ||
                static_cast<char>(data[typeOffset + 1]) != 'A' ||
                static_cast<char>(data[typeOffset + 2]) != 'V' ||
                static_cast<char>(data[typeOffset + 3]) != 'E')
                throw std::runtime_error("Failed to load sound file, not a WAVE file");

            std::uint16_t bitsPerSample = 0U;
            std::uint16_t formatTag = 0U;
            std::vector<std::byte> soundData;

            for (std::size_t offset = typeOffset + 4; offset < data.size();)
            {
                if (data.size() < offset + 8)
                    throw std::runtime_error("Failed to load sound file, not enough data to read chunk");

                const std::byte chunkHeader[4] = {
                    data[offset + 0],
                    data[offset + 1],
                    data[offset + 2],
                    data[offset + 3]
                };

                offset += 4;

                const auto chunkSize = static_cast<std::uint32_t>(data[offset + 0]) |
                    (static_cast<std::uint32_t>(data[offset + 1]) << 8) |
                    (static_cast<std::uint32_t>(data[offset + 2]) << 16) |
                    (static_cast<std::uint32_t>(data[offset + 3]) << 24);

                offset += 4;

                if (data.size() < offset + chunkSize)
                    throw std::runtime_error("Failed to load sound file, not enough data to read chunk");

                if (static_cast<char>(chunkHeader[0]) == 'f' &&
                    static_cast<char>(chunkHeader[1]) == 'm' &&
                    static_cast<char>(chunkHeader[2]) == 't' &&
                    static_cast<char>(chunkHeader[3]) == ' ')
                {
                    if (chunkSize < 16)
                        throw std::runtime_error("Failed to load sound file, not enough data to read chunk");

                    const std::size_t formatTagOffset = offset;

                    formatTag = static_cast<std::uint16_t>(static_cast<std::uint32_t>(data[formatTagOffset + 0]) |
                                                           (static_cast<std::uint32_t>(data[formatTagOffset + 1]) << 8));

                    if (formatTag != WAVE_FORMAT_PCM && formatTag != WAVE_FORMAT_IEEE_FLOAT)
                        throw std::runtime_error("Failed to load sound file, unsupported format");

                    const std::size_t channelsOffset = formatTagOffset + 2;
                    channels = static_cast<std::uint32_t>(data[channelsOffset + 0]) |
                        (static_cast<std::uint32_t>(data[channelsOffset + 1]) << 8);

                    if (!channels)
                        throw std::runtime_error("Failed to load sound file, invalid channel count");

                    const std::size_t sampleRateOffset = channelsOffset + 2;
                    sampleRate = static_cast<std::uint32_t>(data[sampleRateOffset + 0]) |
                        (static_cast<std::uint32_t>(data[sampleRateOffset + 1]) << 8) |
                        (static_cast<std::uint32_t>(data[sampleRateOffset + 2]) << 16) |
                        (static_cast<std::uint32_t>(data[sampleRateOffset + 3]) << 24);

                    if (!sampleRate)
                        throw std::runtime_error("Failed to load sound file, invalid sample rate");

                    const std::size_t byteRateOffset = sampleRateOffset + 4;
                    const std::size_t blockAlignOffset = byteRateOffset + 4;
                    const std::size_t bitsPerSampleOffset = blockAlignOffset + 2;
                    bitsPerSample = static_cast<std::uint16_t>(static_cast<std::uint32_t>(data[bitsPerSampleOffset + 0]) |
                                                               (static_cast<std::uint32_t>(data[bitsPerSampleOffset + 1]) << 8));

                    if (bitsPerSample != 8 && bitsPerSample != 16 &&
                        bitsPerSample != 24 && bitsPerSample != 32)
                        throw std::runtime_error("Failed to load sound file, unsupported bit depth");

                    if (formatTag != WAVE_FORMAT_PCM && formatTag != WAVE_FORMAT_IEEE_FLOAT)
                    {
                        const auto size = static_cast<std::uint32_t>(data[offset + 0]) |
                            (static_cast<std::uint32_t>(data[offset + 1]) << 8) |
                            (static_cast<std::uint32_t>(data[offset + 2]) << 16) |
                            (static_cast<std::uint32_t>(data[offset + 3]) << 24);

                        offset += 4;
                        offset += size; // skip the additional data
                    }
                }
                else if (static_cast<char>(chunkHeader[0]) == 'd' &&
                         static_cast<char>(chunkHeader[1]) == 'a' &&
                         static_cast<char>(chunkHeader[2]) == 't' &&
                         static_cast<char>(chunkHeader[3]) == 'a')
                    soundData.assign(data.begin() + static_cast<int>(offset), data.begin() + static_cast<int>(offset + chunkSize));

                // padding
                offset += ((chunkSize + 1) & 0xFFFFFFFEU);
            }

            if (!formatTag)
                throw std::runtime_error("Failed to load sound file, failed to find a format chunk");

            if (data.empty())
                throw std::runtime_error("Failed to load sound file, failed to find a data chunk");

            const auto sampleCount = static_cast<std::uint32_t>(soundData.size() / (bitsPerSample / 8));
            const auto frames = sampleCount / channels;
            std::vector<float> samples(sampleCount);

            if (formatTag == WAVE_FORMAT_PCM)
            {
                switch (bitsPerSample)
                {
                    case 8:
                    {
                        for (std::uint32_t channel = 0; channel < channels; ++channel)
                        {
                            const auto outputChannel = &samples[channel * frames];

                            for (std::uint32_t frame = 0; frame < frames; ++frame)
                            {
                                const auto sourceData = &soundData[frame * channels + channel];
                                const auto value = static_cast<std::uint8_t>(sourceData[0]);
                                outputChannel[frame] = 2.0F * value / 255.0F - 1.0F;
                            }
                        }
                        break;
                    }
                    case 16:
                    {
                        for (std::uint32_t channel = 0; channel < channels; ++channel)
                        {
                            const auto outputChannel = &samples[channel * frames];

                            for (std::uint32_t frame = 0; frame < frames; ++frame)
                            {
                                const auto sourceData = &soundData[(frame * channels + channel) * 2];
                                const auto value = static_cast<std::int32_t>(sourceData[0]) |
                                    (static_cast<std::int32_t>(sourceData[1]) << 8);
                                outputChannel[frame] = static_cast<float>(value) / 32767.0F;
                            }
                        }
                        break;
                    }
                    case 24:
                    {
                        for (std::uint32_t channel = 0; channel < channels; ++channel)
                        {
                            const auto outputChannel = &samples[channel * frames];

                            for (std::uint32_t frame = 0; frame < frames; ++frame)
                            {
                                const auto sourceData = &soundData[(frame * channels + channel) * 3];
                                const auto value = convert24bitTo232bit(sourceData);

                                outputChannel[frame] = static_cast<float>(value / 8388607.0);
                            }
                        }
                        break;
                    }
                    case 32:
                    {
                        for (std::uint32_t channel = 0; channel < channels; ++channel)
                        {
                            const auto outputChannel = &samples[channel * frames];

                            for (std::uint32_t frame = 0; frame < frames; ++frame)
                            {
                                const auto sourceData = &soundData[(frame * channels + channel) * 4];
                                const auto value = static_cast<std::int32_t>(sourceData[0]) |
                                    (static_cast<std::int32_t>(sourceData[1]) << 8) |
                                    (static_cast<std::int32_t>(sourceData[2]) << 16) |
                                    (static_cast<std::int32_t>(sourceData[3]) << 24);
                                outputChannel[frame] = static_cast<float>(value / 2147483647.0);
                            }
                        }
                        break;
                    }
                    default:
                        throw std::runtime_error("Failed to load sound file, unsupported bit depth");
                }
            }
            else if (formatTag == WAVE_FORMAT_IEEE_FLOAT)
            {
                if (bitsPerSample == 32)
                {
                    for (std::uint32_t channel = 0; channel < channels; ++channel)
                    {
                        const auto outputChannel = &samples[channel * frames];

                        for (std::uint32_t frame = 0; frame < frames; ++frame)
                        {
                            const auto sourceData = &soundData[(frame * channels + channel) * 4];
                            std::memcpy(&outputChannel[frame], sourceData, sizeof(float));
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
}
