// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "SoundInput.hpp"
#include "SoundOutput.hpp"

namespace ouzel
{
    namespace audio
    {
        SoundInput::~SoundInput()
        {
            if (output) output->removeInput(this);
        }

        void SoundInput::setOutput(SoundOutput* newOutput)
        {
            if (output) output->removeInput(this);
            output = newOutput;
            if (output) output->addInput(this);
        }
    } // namespace audio
} // namespace ouzel
