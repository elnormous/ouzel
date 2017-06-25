// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SoundDataWave.h"
#include "core/Engine.h"
#include "files/FileSystem.h"
#include "utils/Log.h"
#include "utils/Utils.h"

namespace ouzel
{
    namespace audio
    {
        SoundDataWave::SoundDataWave()
        {
        }

        SoundDataWave::~SoundDataWave()
        {
        }

        bool SoundDataWave::initFromFile(const std::string& newFilename)
        {
            filename = newFilename;

            std::vector<uint8_t> newData;
            if (!sharedEngine->getFileSystem()->readFile(newFilename, newData))
            {
                return false;
            }

            return initFromBuffer(newData);
        }

        bool SoundDataWave::initFromBuffer(const std::vector<uint8_t>& newData)
        {
            uint32_t offset = 0;

            if (newData.size() < 16) // RIFF + size + WAVE
            {
                Log(Log::Level::ERR) << "Failed to load sound file, file too small";
                return false;
            }

            if (newData[offset + 0] != 'R' ||
                newData[offset + 1] != 'I' ||
                newData[offset + 2] != 'F' ||
                newData[offset + 3] != 'F')
            {
                Log(Log::Level::ERR) << "Failed to load sound file, not a RIFF format";
                return false;
            }

            offset += 4;

            uint32_t length = decodeUInt32Little(newData.data() + offset);

            offset += 4;

            if (newData.size() != length + 8)
            {
                Log(Log::Level::ERR) << "Failed to load sound file, size mismatch";
            }

            if (newData[offset + 0] != 'W' ||
                newData[offset + 1] != 'A' ||
                newData[offset + 2] != 'V' ||
                newData[offset + 3] != 'E')
            {
                Log(Log::Level::ERR) << "Failed to load sound file, not a WAVE file";
                return false;
            }

            offset += 4;

            bool formatChunkFound = false;
            bool dataChunkFound = false;

            uint16_t bitsPerSample = 0;
            std::vector<uint8_t> soundData;

            for (; offset < newData.size();)
            {
                if (newData.size() < offset + 8)
                {
                    Log(Log::Level::ERR) << "Failed to load sound file, not enough data to read chunk";
                    return false;
                }

                uint8_t chunkHeader[4];
                chunkHeader[0] = newData[offset + 0];
                chunkHeader[1] = newData[offset + 1];
                chunkHeader[2] = newData[offset + 2];
                chunkHeader[3] = newData[offset + 3];

                offset += 4;

                uint32_t chunkSize = decodeUInt32Little(newData.data() + offset);
                offset += 4;

                if (newData.size() < offset + chunkSize)
                {
                    Log(Log::Level::ERR) << "Failed to load sound file, not enough data to read chunk";
                    return false;
                }

                if (chunkHeader[0] == 'f' && chunkHeader[1] == 'm' && chunkHeader[2] == 't' && chunkHeader[3] == ' ')
                {
                    if (chunkSize < 16)
                    {
                        Log(Log::Level::ERR) << "Failed to load sound file, not enough data to read chunk";
                        return false;
                    }

                    uint32_t i = offset;

                    formatTag = decodeUInt16Little(newData.data() + i);
                    i += 2;

                    if (formatTag != 1)
                    {
                        Log(Log::Level::ERR) << "Failed to load sound file, bad format tag";
                        return false;
                    }

                    channels = decodeUInt16Little(newData.data() + i);
                    i += 2;

                    samplesPerSecond = decodeUInt32Little(newData.data() + i);
                    i += 4;

                    i += 4; // average bytes per second

                    i += 2; // block align

                    bitsPerSample = decodeUInt16Little(newData.data() + i);
                    i += 2;

                    formatChunkFound = true;
                }
                else if (chunkHeader[0] == 'd' && chunkHeader[1] == 'a' && chunkHeader[2] == 't' && chunkHeader[3] == 'a')
                {
                    soundData.assign(newData.begin() + static_cast<int>(offset), newData.begin() + static_cast<int>(offset + chunkSize));

                    dataChunkFound = true;
                }

                offset += ((chunkSize + 1) & 0xFFFFFFFE);
            }

            if (!formatChunkFound)
            {
                Log(Log::Level::ERR) << "Failed to load sound file, failed to find a format chunk";
                return false;
            }

            if (!dataChunkFound)
            {
                Log(Log::Level::ERR) << "Failed to load sound file, failed to find a data chunk";
                return false;
            }

            if (bitsPerSample != 8 && bitsPerSample != 16 &&
                bitsPerSample != 24 && bitsPerSample != 32)
            {
                Log(Log::Level::ERR) << "Failed to load sound file, unsupported bit depth";
                return false;
            }

            if (bitsPerSample == 16)
            {
                data = std::move(soundData);
            }
            else
            {
                uint32_t bytesPerSample = bitsPerSample / 8;
                uint32_t samples = static_cast<uint32_t>(soundData.size() / bytesPerSample);

                data.reserve(samples * 2);

                for (uint32_t position = 0; position < samples * bytesPerSample; position += bytesPerSample)
                {
                    int16_t sample = 0;

                    if (bitsPerSample < 16) // signed 8-bit sample
                    {
                        sample = static_cast<int16_t>((static_cast<int32_t>(soundData[position]) << (16 - bitsPerSample)) - 32768);
                    }
                    else if (bitsPerSample > 16)
                    {
                        sample = static_cast<int16_t>(static_cast<int32_t>(soundData[position + bytesPerSample - 2]) |
                                                      static_cast<int32_t>(soundData[position + bytesPerSample - 1]) << 8);
                    }

                    // encode sample as little endian integer
                    data.push_back(static_cast<uint8_t>(sample));
                    data.push_back(static_cast<uint8_t>(sample >> 8));
                }
            }

            return true;
        }

        const std::vector<uint8_t>& SoundDataWave::getData()
        {
            return data;
        }
    } // namespace audio
} // namespace ouzel
