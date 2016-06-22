// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "utils/Types.h"

namespace ouzel
{
    namespace audio
    {
        class Audio
        {
        public:
            virtual ~Audio();

            virtual bool init();

            virtual SoundDataPtr createSoundData();
            virtual SoundPtr createSound();

        protected:
            Audio();
        };
    } // namespace audio
} // namespace ouzel
