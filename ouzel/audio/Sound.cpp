// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Sound.hpp"
#include "Audio.hpp"
#include "AudioDevice.hpp"
#include "SoundData.hpp"
#include "Stream.hpp"
#include "core/Engine.hpp"
#include "scene/Actor.hpp"
#include "math/MathUtils.hpp"

namespace ouzel
{
    namespace audio
    {
        Sound::Sound():
            scene::Component(scene::Component::SOUND), playing(false)
        {
        }

        Sound::Sound(const std::shared_ptr<SoundData>& initSoundData):
            Sound()
        {
            soundData = initSoundData;

            if (soundData)
            {
                stream = soundData->createStream();
                stream->setEventListener(this);
            }
        }

        Sound::~Sound()
        {
            if (stream) stream->setEventListener(nullptr);
        }

        Sound::Sound(Sound&& other):
            scene::Component(scene::Component::SOUND)
        {
            soundData = std::move(other.soundData);
            stream = std::move(other.stream);
            if (stream) stream->setEventListener(this);
            position = other.position;
            pitch = other.pitch;
            gain = other.gain;
            rolloffFactor = other.rolloffFactor;
            minDistance = other.minDistance;
            maxDistance = other.maxDistance;
            transformDirty = other.transformDirty;
            spatialized = other.spatialized;
            playing = other.playing.load();
            repeating = other.repeating;

            other.position = Vector3();
            other.pitch = 1.0F;
            other.gain = 1.0F;
            other.rolloffFactor = 1.0F;
            other.minDistance = 1.0F;
            other.maxDistance = FLT_MAX;

            other.transformDirty = true;
            other.spatialized = true;
            other.playing = false;
            other.repeating = false;
        }

        Sound& Sound::operator=(Sound&& other)
        {
            if (&other != this)
            {
                if (stream) stream->setEventListener(nullptr);

                soundData = std::move(other.soundData);
                stream = std::move(other.stream);
                if (stream) stream->setEventListener(this);
                position = other.position;
                pitch = other.pitch;
                gain = other.gain;
                rolloffFactor = other.rolloffFactor;
                minDistance = other.minDistance;
                maxDistance = other.maxDistance;
                transformDirty = other.transformDirty;
                spatialized = other.spatialized;
                playing = other.playing.load();
                repeating = other.repeating;

                other.position = Vector3();
                other.pitch = 1.0F;
                other.gain = 1.0F;
                other.rolloffFactor = 1.0F;
                other.minDistance = 1.0F;
                other.maxDistance = FLT_MAX;

                other.transformDirty = true;
                other.spatialized = true;
                other.playing = false;
                other.repeating = false;
            }

            return *this;
        }

        void Sound::updateTransform()
        {
            transformDirty = true;
        }

        void Sound::setPitch(float newPitch)
        {
            pitch = newPitch;
        }

        void Sound::setGain(float newGain)
        {
            gain = newGain;
        }

        void Sound::setRolloffFactor(float newRolloffFactor)
        {
            rolloffFactor = newRolloffFactor;
        }

        void Sound::setMinDistance(float newMinDistance)
        {
            minDistance = newMinDistance;
        }

        void Sound::setMaxDistance(float newMaxDistance)
        {
            maxDistance = newMaxDistance;
        }

        void Sound::setSpatialized(bool newSpatialized)
        {
            spatialized = newSpatialized;
        }

        void Sound::play(bool repeatSound)
        {
            if (actor) position = actor->getWorldPosition();

            playing = true;
            repeating = repeatSound;

            std::unique_ptr<SoundEvent> startEvent(new SoundEvent());
            startEvent->type = Event::Type::SOUND_START;
            startEvent->sound = this;
            engine->getEventDispatcher().postEvent(std::move(startEvent));

            if (stream)
            {
                stream->setRepeating(repeatSound);
                stream->setPlaying(true);
            }
        }

        void Sound::pause()
        {
            playing = false;
            if (stream) stream->setPlaying(false);
        }

        void Sound::stop()
        {
            playing = false;
            if (stream)
            {
                stream->setPlaying(false);
                stream->setShouldReset(true);
            }
        }

        // executed on audio thread
        void Sound::onReset()
        {
            std::unique_ptr<SoundEvent> event(new SoundEvent());
            event->type = Event::Type::SOUND_RESET;
            event->sound = this;
            engine->getEventDispatcher().postEvent(std::move(event));
        }

        // executed on audio thread
        void Sound::onStop()
        {
            playing = false;

            std::unique_ptr<SoundEvent> event(new SoundEvent());
            event->type = Event::Type::SOUND_FINISH;
            event->sound = this;
            engine->getEventDispatcher().postEvent(std::move(event));
        }
    } // namespace audio
} // namespace ouzel
