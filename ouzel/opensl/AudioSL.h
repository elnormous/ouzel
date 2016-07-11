// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <SLES/OpenSLES_AndroidConfiguration.h>

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

            SLEngineItf getEngine() const { return engine; }
            SLObjectItf getOutputMix() const { return outputMixObject; }

        protected:
            AudioSL();

            SLObjectItf engineObject = nullptr;
            SLEngineItf engine = nullptr;
            SLObjectItf outputMixObject = nullptr;
        };
    } // namespace audio
} // namespace ouzel
