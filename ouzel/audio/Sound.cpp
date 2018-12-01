// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Sound.hpp"
#include "Audio.hpp"
#include "AudioDevice.hpp"
#include "SoundData.hpp"
#include "Stream.hpp"
#include "core/Engine.hpp"

namespace ouzel
{
    namespace audio
    {
        Sound::Sound()
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

        void Sound::play(bool repeatSound)
        {
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

        void Sound::setOutput(Bus* newOutput)
        {
            output = newOutput;
        }
    } // namespace audio
} // namespace ouzel
