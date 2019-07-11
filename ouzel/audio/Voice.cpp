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
            audio.addCommand(std::make_unique<mixer::PlayStreamCommand>(streamId));

            playing = true;

            auto startEvent = std::make_unique<SoundEvent>();
            startEvent->type = Event::Type::SoundStart;
            startEvent->voice = this;
            engine->getEventDispatcher().postEvent(std::move(startEvent));
        }

        void Voice::pause()
        {
            audio.addCommand(std::make_unique<mixer::StopStreamCommand>(streamId, false));

            playing = false;
        }

        void Voice::stop()
        {
            audio.addCommand(std::make_unique<mixer::StopStreamCommand>(streamId, true));

            playing = false;
        }

        // executed on audio thread
        /*void Voice::onReset()
        {
            auto event = std::make_unique<SoundEvent>();
            event->type = Event::Type::SoundReset;
            event->voice = this;
            engine->getEventDispatcher().postEvent(std::move(event));
        }

        // executed on audio thread
        void Voice::onStop()
        {
            playing = false;

            auto event = std::make_unique<SoundEvent>();
            event->type = Event::Type::SoundFinish;
            event->voice = this;
            engine->getEventDispatcher().postEvent(std::move(event));
        }*/

        void Voice::setOutput(Mix* newOutput)
        {
            if (output) output->removeInput(this);
            output = newOutput;
            if (output) output->addInput(this);

            audio.addCommand(std::make_unique<mixer::SetStreamOutputCommand>(streamId,
                                                                             output ? output->getBusId() : 0));
        }
    } // namespace audio
} // namespace ouzel
