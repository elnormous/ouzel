// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "audio/Sound.h"

namespace ouzel
{
    namespace audio
    {
        class AudioEmpty;

        class SoundEmpty: public Sound
        {
            friend AudioEmpty;
        public:
            virtual ~SoundEmpty();

        protected:
            SoundEmpty();
        };
    } // namespace audio
} // namespace ouzel
