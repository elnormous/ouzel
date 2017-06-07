// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "AudioDS.h"
#include "SoundDS.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace audio
    {
        AudioDS::AudioDS():
            Audio(Driver::DIRECTSOUND)
        {
        }

        AudioDS::~AudioDS()
        {
            resourceDeleteSet.clear();
            resources.clear();
        }

        bool AudioDS::init()
        {
            return Audio::init();
        }

        void AudioDS::setListenerPosition(const Vector3& newPosition)
        {
            Audio::setListenerPosition(newPosition);
        }

        void AudioDS::setListenerRotation(const Quaternion& newRotation)
        {
            Audio::setListenerRotation(newRotation);
        }

        SoundResource* AudioDS::createSound()
        {
            SoundResource* sound = new SoundDS();
            resources.push_back(std::unique_ptr<SoundResource>(sound));
            return sound;
        }
    } // namespace audio
} // namespace ouzel
