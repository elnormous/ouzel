// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "audio/Audio.h"

namespace ouzel
{
    class Engine;

    namespace audio
    {
        class AudioVoid: public Audio
        {
            friend Engine;
        public:
            virtual ~AudioVoid();

            virtual SoundDataPtr createSoundData() override;
            virtual SoundPtr createSound() override;

        protected:
            AudioVoid();
        };
    } // namespace audio
} // namespace ouzel
