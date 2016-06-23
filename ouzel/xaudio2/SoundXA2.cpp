// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SoundXA2.h"
#include "AudioXA2.h"
#include "SoundDataXA2.h"
#include "core/Engine.h"
#include "utils/Utils.h"

namespace ouzel
{
    namespace audio
    {
        SoundXA2::SoundXA2()
        {
        }

        SoundXA2::~SoundXA2()
        {
            if (sourceVoice) sourceVoice->DestroyVoice();
        }

        bool SoundXA2::init(const SoundDataPtr& newSoundData)
        {
            if (!Sound::init(newSoundData))
            {
                return false;
            }

            std::shared_ptr<AudioXA2> audioXA2 = std::static_pointer_cast<AudioXA2>(sharedEngine->getAudio());
            std::shared_ptr<SoundDataXA2> soundDataXA2 = std::static_pointer_cast<SoundDataXA2>(soundData);

            if (FAILED(audioXA2->getXAudio()->CreateSourceVoice(&sourceVoice, &soundDataXA2->getWaveFormat())))
            {
                log("Failed to create source voice");
                return false;
            }

            return true;
        }
    } // namespace audio
} // namespace ouzel
