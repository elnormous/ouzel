// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SoundData.h"
#include "core/Engine.h"
#include "utils/Utils.h"

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

        void SoundData::free()
        {
            data.clear();

            ready = false;
        }

        bool SoundData::initFromFile(const std::string& newFilename)
        {
            filename = newFilename;

            std::vector<uint8_t> newData;
            if (!sharedEngine->getFileSystem()->loadFile(newFilename, newData))
            {
                return false;
            }

            return initFromBuffer(newData);
        }

        bool SoundData::initFromBuffer(const std::vector<uint8_t>& newData)
        {
            ready = false;

            uint32_t offset = 0;

            if (newData.size() < 16) // RIFF + size + WAVE
            {
                log("Failed to load sound file. File too small");
                return false;
            }

            if (newData[offset + 0] != 'R' ||
                newData[offset + 1] != 'I' ||
                newData[offset + 2] != 'F' ||
                newData[offset + 3] != 'F')
            {
                log("Failed to load sound file. Not a RIFF format.");
                return false;
            }

            offset += 4;

            uint32_t length = *reinterpret_cast<const uint32_t*>(newData.data() + offset);

            offset += 4;

            if (newData.size() != length + 8)
            {
                log("Failed to load sound file. Size mismatch.");
            }

            if (newData[offset + 0] != 'W' ||
                newData[offset + 1] != 'A' ||
                newData[offset + 2] != 'V' ||
                newData[offset + 3] != 'E')
            {
                log("Failed to load sound file. Not a WAVE file.");
                return false;
            }

            offset += 4;

            bool foundChunkFound = false;

            for (uint32_t i = offset; i < newData.size();)
            {
                if (newData.size() < i + 8)
                {
                    log("Failed to load sound file. Not enough data to read chunk.");
                    return false;
                }

                char chunkHeader[4];
                chunkHeader[0] = newData[i + 0];
                chunkHeader[1] = newData[i + 1];
                chunkHeader[2] = newData[i + 2];
                chunkHeader[3] = newData[i + 3];

                i += 4;

                uint32_t chunkSize = *reinterpret_cast<const uint32_t*>(newData.data() + i);
                i += 4;

                if (newData.size() < i + chunkSize)
                {
                    log("Failed to load sound file. Not enough data to read chunk.");
                    return false;
                }

                if (chunkHeader[0] == 'f' &&
                    chunkHeader[1] == 'm' &&
                    chunkHeader[2] == 't' &&
                    chunkHeader[3] == ' ')
                {
                    if (chunkSize < 16)
                    {
                        log("Failed to load sound file. Not enough data to read chunk.");
                        return false;
                    }

                    formatTag = *reinterpret_cast<const uint16_t*>(newData.data() + i);
                    i += 2;

                    if (formatTag != 1)
                    {
                        log("Failed to load sound file. Bad format tag.");
                        return false;
                    }

                    channels = *reinterpret_cast<const uint16_t*>(newData.data() + i);
                    i += 2;

                    samplesPerSecond = *reinterpret_cast<const uint32_t*>(newData.data() + i);
                    i += 4;

                    averageBytesPerSecond = *reinterpret_cast<const uint32_t*>(newData.data() + i);
                    i += 4;

                    blockAlign = *reinterpret_cast<const uint16_t*>(newData.data() + i);
                    i += 2;

                    bitsPerSample = *reinterpret_cast<const uint16_t*>(newData.data() + i);
                    i += 2;

                    foundChunkFound = true;
                    break;
                }

                i += (chunkSize + 1) & 0xFFFFFFFE;
            }

            if (!foundChunkFound)
            {
                log("Failed to load sound file. Failed to find a format chunk.");
                return false;
            }

            bool dataChunkFound = false;

            for (uint32_t i = offset; i < newData.size();)
            {
                if (newData.size() < i + 8)
                {
                    log("Failed to load sound file. Not enough data to read chunk.");
                    return false;
                }

                char chunkHeader[4];
                chunkHeader[0] = newData[i + 0];
                chunkHeader[1] = newData[i + 1];
                chunkHeader[2] = newData[i + 2];
                chunkHeader[3] = newData[i + 3];

                i += 4;

                uint32_t chunkSize = *reinterpret_cast<const uint32_t*>(newData.data() + i);
                i += 4;

                if (newData.size() < i + chunkSize)
                {
                    log("Failed to load sound file. Not enough data to read chunk.");
                    return false;
                }

                if (chunkHeader[0] == 'd' &&
                    chunkHeader[1] == 'a' &&
                    chunkHeader[2] == 't' &&
                    chunkHeader[3] == 'a')
                {
                    data.assign(newData.begin() + i, newData.begin() + i + chunkSize);

                    dataChunkFound = true;
                    break;
                }

                i += (chunkSize + 1) & 0xFFFFFFFE;
            }

            if (!dataChunkFound)
            {
                log("Failed to load sound file. Failed to find a data chunk.");
                return false;
            }

            ready = true;

            return true;
        }
    } // namespace audio
} // namespace ouzel
