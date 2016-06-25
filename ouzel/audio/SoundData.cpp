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
            const uint32_t HEADER_SIZE = 16; // RIFF + size + WAVEfmt
            const uint32_t PCM_HEADER_SIZE = 20;

            uint64_t offset = 0;

            if (newData.size() < HEADER_SIZE + PCM_HEADER_SIZE)
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
                newData[offset + 3] != 'E' ||
                newData[offset + 4] != 'f' ||
                newData[offset + 5] != 'm' ||
                newData[offset + 6] != 't' ||
                newData[offset + 7] != ' ')
            {
                log("Failed to load sound file. Not a WAVE file.");
                return false;
            }

            offset += 8;

            uint32_t pcmHeaderLength = *reinterpret_cast<const uint32_t*>(newData.data() + offset);
            offset += 4;

            uint16_t formatTag = *reinterpret_cast<const uint16_t*>(newData.data() + offset);
            offset += 2;

            channels = *reinterpret_cast<const uint16_t*>(newData.data() + offset);
            offset += 2;

            samplesPerSecond = *reinterpret_cast<const uint32_t*>(newData.data() + offset);
            offset += 4;

            averageBytesPerSecond = *reinterpret_cast<const uint32_t*>(newData.data() + offset);
            offset += 4;

            blockAlign = *reinterpret_cast<const uint16_t*>(newData.data() + offset);
            offset += 2;

            bitsPerSample = *reinterpret_cast<const uint16_t*>(newData.data() + offset);
            offset += 2;

            if (formatTag != 1)
            {
                log("Failed to load sound file. Bad format tag.");
            }

            offset = HEADER_SIZE + 4 + pcmHeaderLength;

            return true;
        }
    } // namespace audio
} // namespace ouzel
