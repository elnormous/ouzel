// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Source.hpp"
#include "Bus.hpp"

namespace ouzel
{
    namespace audio
    {
        Source::~Source()
        {
            if (output) output->removeInput(this);
        }

        void Source::setOutput(Bus* newOutput)
        {
            if (output) output->removeInput(this);
            output = newOutput;
            if (output) output->addInput(this);
        }
    } // namespace audio
} // namespace ouzel
