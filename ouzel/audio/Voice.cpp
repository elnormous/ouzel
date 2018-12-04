// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Voice.hpp"
#include "Audio.hpp"
#include "AudioDevice.hpp"
#include "Sound.hpp"
#include "core/Engine.hpp"

namespace ouzel
{
    namespace audio
    {
        Voice::Voice(Audio& initAudio, const std::shared_ptr<Sound>& initSound):
            audio(initAudio),
            sourceId(audio.initSource(initSound->getSourceDataId()))
        {
            sound = initSound;
        }

        Voice::~Voice()
        {
        }

        void Voice::play(bool repeat)
        {
            audio.getMixer().addCommand(std::unique_ptr<Command>(new PlaySourceCommand(sourceId, repeat)));

            playing = true;
            repeating = repeat;

            std::unique_ptr<SoundEvent> startEvent(new SoundEvent());
            startEvent->type = Event::Type::SOUND_START;
            startEvent->voice = this;
            engine->getEventDispatcher().postEvent(std::move(startEvent));
        }

        void Voice::pause()
        {
            audio.getMixer().addCommand(std::unique_ptr<Command>(new StopSourceCommand(sourceId, false)));

            playing = false;
        }

        void Voice::stop()
        {
            audio.getMixer().addCommand(std::unique_ptr<Command>(new StopSourceCommand(sourceId, true)));

            playing = false;
        }

        // executed on audio thread
        /*void Voice::onReset()
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
        }*/

        void Voice::setOutput(Mix* newOutput)
        {
            if (output) output->removeInput(this);
            output = newOutput;
            if (output) output->addInput(this);

            audio.getMixer().addCommand(std::unique_ptr<Command>(new SetSourceOutputCommand(sourceId,
                                                                                            output ? output->getBusId() : 0)));
        }
    } // namespace audio
} // namespace ouzel
