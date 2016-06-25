// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "utils/Types.h"

namespace ouzel
{
    class Engine;

    namespace audio
    {
        class Audio
        {
            friend Engine;
        public:
            virtual ~Audio();
            virtual void free();

            virtual bool init();

            virtual SoundDataPtr createSoundData();
            virtual SoundPtr createSound();

            bool isReady() const { return ready; }

        protected:
            Audio();

            bool ready = false;
        };
    } // namespace audio
} // namespace ouzel
