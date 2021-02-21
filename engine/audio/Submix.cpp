// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#include "Submix.hpp"
#include "Audio.hpp"

namespace ouzel::audio
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

        audio.addCommand(std::make_unique<mixer::SetBusOutputCommand>(busId, output ? output->getBusId() : 0));
    }
}
