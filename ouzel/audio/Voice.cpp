// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "Voice.hpp"
#include "Audio.hpp"
#include "AudioDevice.hpp"
#include "Sound.hpp"
#include "core/Engine.hpp"

namespace ouzel
{
    namespace audio
    {
        Voice::Voice(Audio& initAudio):
            Node(initAudio),
            audio(initAudio)
        {
        }

        Voice::Voice(Audio& initAudio, const std::shared_ptr<Sound>& initSound):
            Node(initAudio),
            audio(initAudio),
            streamId(audio.initStream(initSound->getSourceId()))
        {
            sound = initSound;
        }

        Voice::~Voice()
        {
            if (streamId)
                audio.deleteObject(streamId);
        }

        void Voice::play()
        {
            audio.addCommand(std::unique_ptr<mixer::Command>(new mixer::PlayStreamCommand(streamId)));

            playing = true;

            std::unique_ptr<SoundEvent> startEvent(new SoundEvent());
            startEvent->type = Event::Type::SOUND_START;
            startEvent->voice = this;
            engine->getEventDispatcher().postEvent(std::move(startEvent));
        }

        void Voice::pause()
        {
            audio.addCommand(std::unique_ptr<mixer::Command>(new mixer::StopStreamCommand(streamId, false)));

            playing = false;
        }

        void Voice::stop()
        {
            audio.addCommand(std::unique_ptr<mixer::Command>(new mixer::StopStreamCommand(streamId, true)));

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

            audio.addCommand(std::unique_ptr<mixer::Command>(new mixer::SetStreamOutputCommand(streamId,
                                                                                               output ? output->getBusId() : 0)));
        }
    } // namespace audio
} // namespace ouzel
