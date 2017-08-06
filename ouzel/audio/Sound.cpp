// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Sound.hpp"
#include "SoundResource.hpp"
#include "Audio.hpp"
#include "core/Engine.hpp"

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

            sharedEngine->getAudio()->executeOnAudioThread(std::bind(&SoundResource::init,
                                                                     resource,
                                                                     newSoundData,
                                                                     relativePosition));

            return true;
        }

        void Sound::setPosition(const Vector3& newPosition)
        {
            sharedEngine->getAudio()->executeOnAudioThread(std::bind(&SoundResource::setPosition,
                                                                     resource,
                                                                     newPosition));
        }

        void Sound::setPitch(float newPitch)
        {
            sharedEngine->getAudio()->executeOnAudioThread(std::bind(&SoundResource::setPitch,
                                                                     resource,
                                                                     newPitch));
        }

        void Sound::setGain(float newGain)
        {
            sharedEngine->getAudio()->executeOnAudioThread(std::bind(&SoundResource::setGain,
                                                                     resource,
                                                                     newGain));
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

        bool Sound::isRepeating() const
        {
            return repeating;
        }
    } // namespace audio
} // namespace ouzel
