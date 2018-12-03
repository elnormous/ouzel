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
        Sound::Sound(Audio& initAudio):
            audio(initAudio),
            sourceId(audio.initSource())
        {
        }

        Sound::Sound(Audio& initAudio, const std::shared_ptr<SoundData>& initSoundData):
            Sound(initAudio)
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

        void Sound::setOutput(Mix* newOutput)
        {
            if (output) output->removeInput(this);
            output = newOutput;
            if (output) output->addInput(this);

            Mixer::Command command(Mixer::Command::Type::SET_SOURCE_OUTPUT);
            command.sourceId = sourceId;
            command.outputBusId = output ? output->getBusId() : 0;
            audio.getMixer().addCommand(command);
        }
    } // namespace audio
} // namespace ouzel
