// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Voice.hpp"
#include "Audio.hpp"
#include "AudioDevice.hpp"
#include "Sound.hpp"
#include "Stream.hpp"
#include "core/Engine.hpp"

namespace ouzel
{
    namespace audio
    {
        Voice::Voice(Audio& initAudio):
            audio(initAudio),
            sourceId(audio.initSource())
        {
        }

        Voice::Voice(Audio& initAudio, const std::shared_ptr<Sound>& initSound):
            Voice(initAudio)
        {
            sound = initSound;

            if (sound)
            {
                stream = sound->createStream();
                stream->setEventListener(this);
            }
        }

        Voice::~Voice()
        {
            if (stream) stream->setEventListener(nullptr);
        }

        void Voice::play(bool repeatSound)
        {
            playing = true;
            repeating = repeatSound;

            std::unique_ptr<SoundEvent> startEvent(new SoundEvent());
            startEvent->type = Event::Type::SOUND_START;
            startEvent->voice = this;
            engine->getEventDispatcher().postEvent(std::move(startEvent));

            if (stream)
            {
                stream->setRepeating(repeatSound);
                stream->setPlaying(true);
            }
        }

        void Voice::pause()
        {
            playing = false;
            if (stream) stream->setPlaying(false);
        }

        void Voice::stop()
        {
            playing = false;
            if (stream)
            {
                stream->setPlaying(false);
                stream->setShouldReset(true);
            }
        }

        // executed on audio thread
        void Voice::onReset()
        {
            std::unique_ptr<SoundEvent> event(new SoundEvent());
            event->type = Event::Type::SOUND_RESET;
            event->voice = this;
            engine->getEventDispatcher().postEvent(std::move(event));
        }

        // executed on audio thread
        void Voice::onStop()
        {
            playing = false;

            std::unique_ptr<SoundEvent> event(new SoundEvent());
            event->type = Event::Type::SOUND_FINISH;
            event->voice = this;
            engine->getEventDispatcher().postEvent(std::move(event));
        }

        void Voice::setOutput(Mix* newOutput)
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
