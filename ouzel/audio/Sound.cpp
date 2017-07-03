// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Sound.h"
#include "SoundResource.h"
#include "core/Engine.h"

namespace ouzel
{
    namespace audio
    {
        Sound::Sound()
        {
            resource = sharedEngine->getAudio()->createSound();
        }

        Sound::~Sound()
        {
            if (sharedEngine) sharedEngine->getAudio()->deleteResource(resource);
        }

        bool Sound::init(const std::shared_ptr<SoundData>& newSoundData, bool newStreaming)
        {
            soundData = newSoundData;
            
            return resource->init(newSoundData, newStreaming);
        }

        void Sound::setPosition(const Vector3& newPosition)
        {
            return resource->setPosition(newPosition);
        }

        void Sound::setPitch(float newPitch)
        {
            return resource->setPitch(newPitch);
        }

        void Sound::setGain(float newGain)
        {
            return resource->setGain(newGain);
        }

        bool Sound::play(bool repeatSound)
        {
            return resource->play(repeatSound);
        }

        bool Sound::pause()
        {
            return resource->pause();
        }

        bool Sound::stop()
        {
            return resource->stop();
        }

        bool Sound::isRepeating() const
        {
            return resource->isRepeating();
        }
    } // namespace audio
} // namespace ouzel
