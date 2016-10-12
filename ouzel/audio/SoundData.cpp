// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SoundData.h"
#include "core/Application.h"
#include "files/FileSystem.h"
#include "utils/Log.h"
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
            if (!sharedApplication->getFileSystem()->loadFile(newFilename, newData))
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

            uint32_t length = readUInt32Little(newData.data() + offset);

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

                uint32_t chunkSize = readUInt32Little(newData.data() + offset);
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

                    formatTag = readUInt16Little(newData.data() + i);
                    i += 2;

                    if (formatTag != 1)
                    {
                        Log(Log::Level::ERR) << "Failed to load sound file, bad format tag";
                        return false;
                    }

                    channels = readUInt16Little(newData.data() + i);
                    i += 2;

                    samplesPerSecond = readUInt32Little(newData.data() + i);
                    i += 4;

                    averageBytesPerSecond = readUInt32Little(newData.data() + i);
                    i += 4;

                    blockAlign = readUInt16Little(newData.data() + i);
                    i += 2;

                    bitsPerSample = readUInt16Little(newData.data() + i);
                    i += 2;

                    formatChunkFound = true;
                }
                else if (chunkHeader[0] == 'd' && chunkHeader[1] == 'a' && chunkHeader[2] == 't' && chunkHeader[3] == 'a')
                {
                    data.assign(newData.begin() + static_cast<int>(offset), newData.begin() + static_cast<int>(offset + chunkSize));

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

            ready = true;

            return true;
        }
    } // namespace audio
} // namespace ouzel
