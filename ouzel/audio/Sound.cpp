// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Sound.hpp"
#include "SoundResource.hpp"
#include "Audio.hpp"
#include "AudioDevice.hpp"
#include "core/Engine.hpp"

namespace ouzel
{
    namespace audio
    {
        Sound::Sound()
        {
            resource = sharedEngine->getAudio()->getDevice()->createSound();
        }

        Sound::~Sound()
        {
            if (sharedEngine) sharedEngine->getAudio()->getDevice()->deleteResource(resource);
        }

        bool Sound::init(const std::shared_ptr<SoundData>& newSoundData, bool newRelativePosition)
        {
            soundData = newSoundData;
            relativePosition = newRelativePosition;

            sharedEngine->getAudio()->executeOnAudioThread(std::bind(&SoundResource::init,
                                                                     resource,
                                                                     newSoundData,
                                                                     newRelativePosition));

            return true;
        }

        void Sound::setPosition(const Vector3& newPosition)
        {
            position = newPosition;

            sharedEngine->getAudio()->executeOnAudioThread(std::bind(&SoundResource::setPosition,
                                                                     resource,
                                                                     newPosition));
        }

        void Sound::setPitch(float newPitch)
        {
            pitch = newPitch;

            sharedEngine->getAudio()->executeOnAudioThread(std::bind(&SoundResource::setPitch,
                                                                     resource,
                                                                     newPitch));
        }

        void Sound::setGain(float newGain)
        {
            gain = newGain;

            sharedEngine->getAudio()->executeOnAudioThread(std::bind(&SoundResource::setGain,
                                                                     resource,
                                                                     newGain));
        }

        void Sound::setRolloffFactor(float newRolloffFactor)
        {
            rolloffFactor = newRolloffFactor;

            sharedEngine->getAudio()->executeOnAudioThread(std::bind(&SoundResource::setGain,
                                                                     resource,
                                                                     newRolloffFactor));
        }

        void Sound::setMinDistance(float newMinDistance)
        {
            minDistance = newMinDistance;

            sharedEngine->getAudio()->executeOnAudioThread(std::bind(&SoundResource::setMinDistance,
                                                                     resource,
                                                                     newMinDistance));
        }

        void Sound::setMaxDistance(float newMaxDistance)
        {
            maxDistance = newMaxDistance;

            sharedEngine->getAudio()->executeOnAudioThread(std::bind(&SoundResource::setMaxDistance,
                                                                     resource,
                                                                     newMaxDistance));
        }

        bool Sound::play(bool repeatSound)
        {
            repeating = repeatSound;

            sharedEngine->getAudio()->executeOnAudioThread(std::bind(&SoundResource::play,
                                                                     resource,
                                                                     repeatSound));
            return true;
        }

        bool Sound::pause()
        {
            sharedEngine->getAudio()->executeOnAudioThread(std::bind(&SoundResource::pause,
                                                                     resource));

            return true;
        }

        bool Sound::stop()
        {
            sharedEngine->getAudio()->executeOnAudioThread(std::bind(&SoundResource::stop,
                                                                     resource));

            return true;
        }

        void Sound::setRelativePosition(bool newRelativePosition)
        {
            relativePosition = newRelativePosition;

            sharedEngine->getAudio()->executeOnAudioThread(std::bind(&SoundResource::setRelativePosition,
                                                                     resource,
                                                                     newRelativePosition));
        }
    } // namespace audio
} // namespace ouzel
