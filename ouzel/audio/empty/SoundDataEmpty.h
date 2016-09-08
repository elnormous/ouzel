// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "audio/SoundData.h"

namespace ouzel
{
    namespace audio
    {
        class AudioEmpty;

        class SoundDataEmpty: public SoundData
        {
            friend AudioEmpty;
        public:
            virtual ~SoundDataEmpty();

        protected:
            SoundDataEmpty();
        };
    } // namespace audio
} // namespace ouzel
