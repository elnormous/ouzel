// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "audio/Sound.h"

namespace ouzel
{
    namespace audio
    {
        class AudioAL;

        class SoundAL: public Sound
        {
            friend AudioAL;
        public:
            virtual ~SoundAL();
            virtual void free() override;

            virtual bool init(const SoundDataPtr& newSoundData) override;

            virtual bool play(bool repeatSound = false) override;
            virtual bool stop(bool resetSound = false) override;
            virtual bool reset() override;

            uint32_t offset = 0;

        protected:
            SoundAL();
        };
    } // namespace audio
} // namespace ouzel
