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
            virtual void free() override;

            virtual bool init(const SoundDataPtr& newSoundData) override;

            uint32_t offset = 0;

        protected:
            SoundCA();

            int audioUnitOpened = 0;

#if OUZEL_PLATFORM_MACOS
            AudioDeviceID deviceID;
            AudioComponentInstance audioUnit;
#else
            AudioUnit audioUnit;
#endif
        };
    } // namespace audio
} // namespace ouzel
