// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_OPENSL

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

            virtual SoundResource* createSound() override;

            SLEngineItf getEngine() const { return engine; }
            SLObjectItf getOutputMix() const { return outputMixObject; }

        protected:
            AudioSL();
            virtual bool init() override;

            SLObjectItf engineObject = nullptr;
            SLEngineItf engine = nullptr;
            SLObjectItf outputMixObject = nullptr;
        };
    } // namespace audio
} // namespace ouzel

#endif
