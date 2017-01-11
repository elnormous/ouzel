// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SoundDataSL.h"

namespace ouzel
{
    namespace audio
    {
        SoundDataSL::SoundDataSL()
        {
        }

        SoundDataSL::~SoundDataSL()
        {
        }

        bool SoundDataSL::initFromBuffer(const std::vector<uint8_t>& newData)
        {
            if (!SoundData::initFromBuffer(newData))
            {
                return false;
            }

            return true;
        }
    } // namespace audio
} // namespace ouzel
