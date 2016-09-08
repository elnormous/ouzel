// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "audio/Sound.h"

namespace ouzel
{
    namespace audio
    {
        class AudioVoid;

        class SoundVoid: public Sound
        {
            friend AudioVoid;
        public:
            virtual ~SoundVoid();

        protected:
            SoundVoid();
        };
    } // namespace audio
} // namespace ouzel
