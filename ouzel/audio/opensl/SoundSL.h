// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
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

            virtual bool init(const std::shared_ptr<SoundData>& newSoundData) override;

            virtual void setGain(float newGain) override;

            virtual bool play(bool repeatSound = false) override;
            virtual bool stop(bool resetSound = false) override;
            virtual bool reset() override;

            SLPlayItf getPlayer() const { return player; }
            const std::vector<uint8_t>& getBuffer() const { return buffer; }

        protected:
            SLObjectItf playerObject = nullptr;
            SLPlayItf player = nullptr;
            SLAndroidSimpleBufferQueueItf bufferQueue = nullptr;
            SLVolumeItf playerVolume = nullptr;
            std::vector<uint8_t> buffer;
        };
    } // namespace audio
} // namespace ouzel
