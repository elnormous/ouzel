// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "audio/SoundData.h"

namespace ouzel
{
    namespace audio
    {
        class AudioCA;

        class SoundDataCA: public SoundData
        {
            friend AudioCA;
        public:
            virtual ~SoundDataCA();

        protected:
            SoundDataCA();
        };
    } // namespace audio
} // namespace ouzel
