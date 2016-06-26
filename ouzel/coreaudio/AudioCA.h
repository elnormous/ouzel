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

#include "audio/Audio.h"

namespace ouzel
{
    namespace audio
    {
        class AudioCA: public Audio
        {
            friend Engine;
        public:
            virtual ~AudioCA();
            virtual void free() override;

            virtual bool init() override;

            virtual SoundDataPtr createSoundData() override;
            virtual SoundPtr createSound() override;

#if OUZEL_PLATFORM_MACOS
            AudioDeviceID getAudioDeviceId() const { return deviceId; }
#endif
            AudioComponent getAudioComponent() const { return audioComponent; }

        protected:
            AudioCA();

#if OUZEL_PLATFORM_MACOS
            AudioDeviceID deviceId;
#endif
            AudioComponent audioComponent;
        };
    } // namespace audio
} // namespace ouzel
