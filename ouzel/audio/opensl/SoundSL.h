// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <SLES/OpenSLES_AndroidConfiguration.h>

#include "audio/Sound.h"

namespace ouzel
{
    namespace audio
    {
        class SoundSL: public Sound
        {
        public:
            SoundSL();
            virtual ~SoundSL();
            virtual void free() override;

            virtual bool init(const SoundDataPtr& newSoundData) override;

            virtual bool play(bool repeatSound = false) override;
            virtual bool stop(bool resetSound = false) override;
            virtual bool reset() override;

            SLPlayItf getPlayer() const { return player; }

        protected:
            SLObjectItf playerObject = nullptr;
            SLPlayItf player = nullptr;
            SLAndroidSimpleBufferQueueItf bufferQueue = nullptr;
            SLVolumeItf playerVolume = nullptr;
        };
    } // namespace audio
} // namespace ouzel
