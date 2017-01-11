// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "audio/SoundData.h"

namespace ouzel
{
    namespace audio
    {
        class SoundDataAL: public SoundData
        {
        public:
            SoundDataAL();
            virtual ~SoundDataAL();

            virtual bool initFromBuffer(const std::vector<uint8_t>& newData);
        };
    } // namespace audio
} // namespace ouzel
