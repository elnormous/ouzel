// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_PLATFORM_MACOS
#include <CoreAudio/CoreAudio.h>
#include <CoreServices/CoreServices.h>
#else
#include <AudioToolbox/AudioToolbox.h>
#endif

#include <AudioUnit/AudioUnit.h>

#include "audio/Sound.h"

namespace ouzel
{
    namespace audio
    {
        class AudioCA;

        class SoundCA: public Sound
        {
            friend AudioCA;
        public:
            virtual ~SoundCA();

            virtual bool init(const SoundDataPtr& newSoundData) override;

        protected:
            SoundCA();
        };
    } // namespace audio
} // namespace ouzel
