// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "audio/SoundData.h"

namespace ouzel
{
    namespace audio
    {
        class AudioSL;

        class SoundDataSL: public SoundData
        {
            friend AudioSL;
        public:
            virtual ~SoundDataSL();

            virtual bool initFromBuffer(const std::vector<uint8_t>& newData);

        protected:
            SoundDataSL();
        };
    } // namespace audio
} // namespace ouzel
