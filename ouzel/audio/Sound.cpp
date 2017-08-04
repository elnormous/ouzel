// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Sound.h"
#include "SoundResource.h"
#include "Audio.h"
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

        bool Sound::init(const std::shared_ptr<SoundData>& newSoundData, bool relativePosition)
        {
            soundData = newSoundData;

            SoundResource* soundResource = resource;

            sharedEngine->getAudio()->executeOnAudioThread([soundResource,
                                                            newSoundData,
                                                            relativePosition]() {
                soundResource->init(newSoundData, relativePosition);
            });

            return true;
        }

        void Sound::setPosition(const Vector3& newPosition)
        {
            SoundResource* soundResource = resource;

            sharedEngine->getAudio()->executeOnAudioThread([soundResource,
                                                            newPosition]() {
                soundResource->setPosition(newPosition);
            });
        }

        void Sound::setPitch(float newPitch)
        {
            SoundResource* soundResource = resource;

            sharedEngine->getAudio()->executeOnAudioThread([soundResource,
                                                            newPitch]() {
                soundResource->setPitch(newPitch);
            });
        }

        void Sound::setGain(float newGain)
        {
            SoundResource* soundResource = resource;

            sharedEngine->getAudio()->executeOnAudioThread([soundResource,
                                                            newGain]() {
                soundResource->setGain(newGain);
            });
        }

        bool Sound::play(bool repeatSound)
        {
            repeating = repeatSound;

            SoundResource* soundResource = resource;

            sharedEngine->getAudio()->executeOnAudioThread([soundResource,
                                                            repeatSound]() {
                soundResource->play(repeatSound);
            });

            return true;
        }

        bool Sound::pause()
        {
            SoundResource* soundResource = resource;

            sharedEngine->getAudio()->executeOnAudioThread([soundResource]() {
                soundResource->pause();
            });

            return true;
        }

        bool Sound::stop()
        {
            SoundResource* soundResource = resource;

            sharedEngine->getAudio()->executeOnAudioThread([soundResource]() {
                soundResource->stop();
            });

            return true;
        }

        bool Sound::isRepeating() const
        {
            return resource->isRepeating();

            return repeating;
        }
    } // namespace audio
} // namespace ouzel
