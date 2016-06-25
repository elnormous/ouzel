// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "utils/Types.h"

namespace ouzel
{
    namespace audio
    {
        class Sound
        {
            friend Audio;
        public:
            virtual ~Sound();

            virtual bool init(const SoundDataPtr& newSoundData);

            virtual void play(bool repeatSound = false);
            virtual void stop(bool resetSound = false);
            virtual void reset();

        protected:
            Sound();

            SoundDataPtr soundData;
            bool repeat = false;
        };
    } // namespace audio
} // namespace ouzel
