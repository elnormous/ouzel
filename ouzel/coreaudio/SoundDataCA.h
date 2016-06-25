// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#if OUZEL_PLATFORM_MACOS
#include <CoreAudio/CoreAudio.h>
#include <CoreServices/CoreServices.h>
#else
#include <AudioToolbox/AudioToolbox.h>
#endif

#include <AudioUnit/AudioUnit.h>

#include "audio/SoundData.h"

namespace ouzel
{
    namespace audio
    {
        class AudioCA;

        class SoundDataCA: public SoundData
        {
            friend AudioCA;
        public:
            virtual ~SoundDataCA();

            virtual bool initFromBuffer(const std::vector<uint8_t>& newData);
            
            const AudioStreamBasicDescription& getStreamDescription() const { return streamDescription; }

        protected:
            SoundDataCA();

            AudioStreamBasicDescription streamDescription;
        };
    } // namespace audio
} // namespace ouzel
