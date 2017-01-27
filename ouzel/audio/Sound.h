// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "utils/Types.h"

namespace ouzel
{
    namespace audio
    {
        class Audio;

        class Sound
        {
            friend Audio;
        public:
            virtual ~Sound();

            virtual bool init(const SoundDataPtr& newSoundData);

            const SoundDataPtr& getSoundData() const { return soundData; }

            virtual bool play(bool repeatSound = false);
            virtual bool stop(bool resetSound = false);
            virtual bool reset();

            bool isRepeating() const { return repeat; }

            bool isReady() const { return ready; }

        protected:
            Sound();

            SoundDataPtr soundData;
            bool repeat = false;

            bool ready = false;
        };
    } // namespace audio
} // namespace ouzel
