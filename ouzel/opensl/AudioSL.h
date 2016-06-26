// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "audio/Audio.h"

namespace ouzel
{
    namespace audio
    {
        class AudioSL: public Audio
        {
            friend Engine;
        public:
            virtual ~AudioSL();
            virtual void free() override;

            virtual bool init() override;

            virtual SoundDataPtr createSoundData() override;
            virtual SoundPtr createSound() override;

        protected:
            AudioSL();
        };
    } // namespace audio
} // namespace ouzel
