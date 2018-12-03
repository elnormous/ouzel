// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

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

        Submix::~Submix()
        {
            if (output) output->removeInput(this);
        }

        void Submix::setOutput(Mix* newOutput)
        {
            if (output) output->removeInput(this);
            output = newOutput;
            output->addInput(this);

            Mixer::Command command(Mixer::Command::Type::SET_BUS_OUTPUT);
            command.busId = busId;
            command.outputBusId = output ? output->getBusId() : 0;
            audio.getMixer().addCommand(command);
        }
    } // namespace audio
} // namespace ouzel
