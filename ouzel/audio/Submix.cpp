// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "Submix.hpp"
#include "Audio.hpp"

namespace ouzel
{
    namespace audio
    {
        Submix::Submix(Audio& initAudio):
            Mix(initAudio)
        {
        }

        Submix::Submix(Audio& initAudio, SubmixDefinition initSubmixDefinition):
            Mix(initAudio),
            submixDefinition(initSubmixDefinition)
        {
            // TODO: create effects
            // TODO: send init command
        }

        Submix::~Submix()
        {
            if (output) output->removeInput(this);
        }

        void Submix::setOutput(Mix* newOutput)
        {
            if (output) output->removeInput(this);
            output = newOutput;
            output->addInput(this);

            audio.addCommand(std::make_unique<mixer::SetBusOutputCommand>(busId, output ? output->getBusId() : 0));
        }
    } // namespace audio
} // namespace ouzel
