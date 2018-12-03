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

            audio.getMixer().addCommand(std::unique_ptr<Command>(new SetBusOutputCommand(busId,
                                                                                         output ? output->getBusId() : 0)));
        }
    } // namespace audio
} // namespace ouzel
