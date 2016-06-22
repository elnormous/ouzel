// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SoundData.h"
#include "core/Engine.h"

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
            return true;
        }
    } // namespace audio
} // namespace ouzel
