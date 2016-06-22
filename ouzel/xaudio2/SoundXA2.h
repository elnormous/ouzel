// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "audio/Sound.h"

namespace ouzel
{
    namespace audio
    {
        class AudioXA2;

        class SoundXA2: public Sound
        {
            friend AudioXA2;
        public:
            virtual ~SoundXA2();

        protected:
            SoundXA2();
        };
    } // namespace audio
} // namespace ouzel
