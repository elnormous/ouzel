// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "audio/SoundData.h"

namespace ouzel
{
    namespace audio
    {
        class AudioXA2;

        class SoundDataXA2: public SoundData
        {
            friend AudioXA2;
        public:
            virtual ~SoundDataXA2();

        protected:
            SoundDataXA2();
        };
    } // namespace audio
} // namespace ouzel
