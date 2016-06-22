// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "audio/Audio.h"

namespace ouzel
{
    namespace audio
    {
        class AudioXA2: public Audio
        {
            friend Engine;
        public:
            virtual ~AudioXA2();

            virtual bool init() override;

            virtual SoundDataPtr createSoundData() override;
            virtual SoundPtr createSound() override;

        protected:
            AudioXA2();
        };
    } // namespace audio
} // namespace ouzel
