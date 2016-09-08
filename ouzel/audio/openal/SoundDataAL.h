// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "audio/SoundData.h"

namespace ouzel
{
    namespace audio
    {
        class AudioAL;

        class SoundDataAL: public SoundData
        {
            friend AudioAL;
        public:
            virtual ~SoundDataAL();

            virtual bool initFromBuffer(const std::vector<uint8_t>& newData);

        protected:
            SoundDataAL();
        };
    } // namespace audio
} // namespace ouzel
