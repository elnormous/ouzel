// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SoundDataAL.h"

namespace ouzel
{
    namespace audio
    {
        SoundDataAL::SoundDataAL()
        {
        }

        SoundDataAL::~SoundDataAL()
        {
        }

        bool SoundDataAL::initFromBuffer(const std::vector<uint8_t>& newData)
        {
            if (!SoundData::initFromBuffer(newData))
            {
                return false;
            }
            
            return true;
        }

    } // namespace audio
} // namespace ouzel
