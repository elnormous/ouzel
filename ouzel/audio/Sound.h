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
            virtual void free();

            virtual bool init(const SoundDataPtr& newSoundData);

            virtual bool play(bool repeatSound = false);
            virtual bool stop(bool resetSound = false);
            virtual bool reset();

            bool isReady() const { return ready; }

        protected:
            Sound();

            SoundDataPtr soundData;
            bool repeat = false;

            bool ready = false;
        };
    } // namespace audio
} // namespace ouzel
