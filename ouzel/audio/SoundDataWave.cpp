// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <iterator>
#include <stdexcept>
#include "SoundDataWave.hpp"
#include "StreamWave.hpp"
#include "utils/Utils.hpp"

enum WaveFormat
{
    PCM = 1,
    IEEE_FLOAT = 3
};

namespace ouzel
{
    namespace audio
    {
        SoundDataWave::SoundDataWave()
        {
        }

        SoundDataWave::SoundDataWave(const std::vector<uint8_t>& initData)
        {
            uint32_t offset = 0;

            if (initData.size() < 12) // RIFF + size + WAVE
                throw std::runtime_error("Failed to load sound file, file too small");

            if (initData[offset + 0] != 'R' ||
                initData[offset + 1] != 'I' ||
                initData[offset + 2] != 'F' ||
                initData[offset + 3] != 'F')
                throw std::runtime_error("Failed to load sound file, not a RIFF format");

            offset += 4;

            uint32_t length = decodeUInt32Little(initData.data() + offset);

            offset += 4;

            if (initData.size() < offset + length)
                throw std::runtime_error("Failed to load sound file, size mismatch");

            if (length < 4 ||
                initData[offset + 0] != 'W' ||
                initData[offset + 1] != 'A' ||
                initData[offset + 2] != 'V' ||
                initData[offset + 3] != 'E')
                throw std::runtime_error("Failed to load sound file, not a WAVE file");

            offset += 4;

            bool formatChunkFound = false;
            bool dataChunkFound = false;

            uint16_t bitsPerSample = 0;
            uint16_t formatTag = 0;
            std::vector<uint8_t> soundData;

            while (offset < initData.size())
            {
                if (initData.size() < offset + 8)
                    throw std::runtime_error("Failed to load sound file, not enough data to read chunk");

                uint8_t chunkHeader[4];
                chunkHeader[0] = initData[offset + 0];
                chunkHeader[1] = initData[offset + 1];
                chunkHeader[2] = initData[offset + 2];
                chunkHeader[3] = initData[offset + 3];

                offset += 4;

                uint32_t chunkSize = decodeUInt32Little(initData.data() + offset);
                offset += 4;

                if (initData.size() < offset + chunkSize)
                    throw std::runtime_error("Failed to load sound file, not enough data to read chunk");

                if (chunkHeader[0] == 'f' && chunkHeader[1] == 'm' && chunkHeader[2] == 't' && chunkHeader[3] == ' ')
                {
                    if (chunkSize < 16)
                        throw std::runtime_error("Failed to load sound file, not enough data to read chunk");

                    uint32_t i = offset;

                    formatTag = decodeUInt16Little(initData.data() + i);
                    i += 2;

                    channels = decodeUInt16Little(initData.data() + i);
                    i += 2;

                    sampleRate = decodeUInt32Little(initData.data() + i);
                    i += 4;

                    i += 4; // average bytes per second

                    i += 2; // block align

                    bitsPerSample = decodeUInt16Little(initData.data() + i);
                    i += 2;

                    formatChunkFound = true;
                }
                else if (chunkHeader[0] == 'd' && chunkHeader[1] == 'a' && chunkHeader[2] == 't' && chunkHeader[3] == 'a')
                {
                    soundData.assign(initData.begin() + static_cast<int>(offset), initData.begin() + static_cast<int>(offset + chunkSize));

                    dataChunkFound = true;
                }

                offset += ((chunkSize + 1) & 0xFFFFFFFE);
            }

            if (!formatChunkFound)
                throw std::runtime_error("Failed to load sound file, failed to find a format chunk");

            if (!dataChunkFound)
                throw std::runtime_error("Failed to load sound file, failed to find a data chunk");

            if (bitsPerSample != 8 && bitsPerSample != 16 &&
                bitsPerSample != 24 && bitsPerSample != 32)
                throw std::runtime_error("Failed to load sound file, unsupported bit depth");

            uint32_t bytesPerSample = bitsPerSample / 8;
            uint32_t samples = static_cast<uint32_t>(soundData.size() / bytesPerSample);
            data.resize(samples);

            if (formatTag == PCM)
            {
                if (bitsPerSample == 8)
                {
                    for (uint32_t i = 0; i < samples; ++i)
                        data[i] = 2.0F * static_cast<float>(soundData[i]) / 255.0F - 1.0F;
                }
                else if (bitsPerSample == 16)
                {
                    for (uint32_t i = 0; i < samples; ++i)
                        data[i] = static_cast<float>(static_cast<int16_t>(soundData[i * 2] |
                                                                          (soundData[i * 2 + 1] << 8))) / 32767.0F;
                }
                else if (bitsPerSample == 24)
                {
                    for (uint32_t i = 0; i < samples; ++i)
                        data[i] = static_cast<float>(static_cast<int32_t>((soundData[i * 3] << 8) |
                                                                          (soundData[i * 3 + 1] << 16) |
                                                                          (soundData[i * 3 + 2] << 24))) / 2147483648.0F;
                }
                else
                    throw std::runtime_error("Failed to load sound file, unsupported bit depth");
            }
            else if (formatTag == IEEE_FLOAT)
            {
                if (bitsPerSample == 32)
                {
                    for (uint32_t i = 0; i < samples; ++i)
                        data[i] = reinterpret_cast<float*>(soundData.data())[i];
                }
                else
                    throw std::runtime_error("Failed to load sound file, unsupported bit depth");
            }
            else
                throw std::runtime_error("Failed to load sound file, unsupported format");
        }

        std::shared_ptr<Stream> SoundDataWave::createStream()
        {
            return std::make_shared<StreamWave>();
        }

        void SoundDataWave::readData(Stream* stream, uint32_t frames, std::vector<float>& result)
        {
            StreamWave* streamWave = static_cast<StreamWave*>(stream);

            uint32_t neededSize = frames * channels;
            uint32_t totalSize = 0;

            result.resize(neededSize);

            while (neededSize > 0)
            {
                if (stream->isRepeating() && (data.size() - streamWave->getOffset()) == 0) streamWave->reset();

                if (data.size() - streamWave->getOffset() < neededSize)
                {
                    std::copy(data.begin() + streamWave->getOffset(),
                              data.end(),
                              result.begin() + totalSize);
                    totalSize += static_cast<uint32_t>(data.size() - streamWave->getOffset());
                    neededSize -= static_cast<uint32_t>(data.size() - streamWave->getOffset());
                    streamWave->setOffset(static_cast<uint32_t>(data.size()));
                }
                else
                {
                    std::copy(data.begin() + streamWave->getOffset(),
                              data.begin() + streamWave->getOffset() + neededSize,
                              result.begin() + totalSize);
                    totalSize += neededSize;
                    streamWave->setOffset(streamWave->getOffset() + neededSize);
                    neededSize = 0;
                }

                if (!stream->isRepeating()) break;
            }

            if ((data.size() - streamWave->getOffset()) == 0) streamWave->reset();

            std::fill(result.begin() + totalSize, result.end(), 0.0F);
        }
    } // namespace audio
} // namespace ouzel
