// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "Stream.hpp"
#include "Bus.hpp"
#include "Source.hpp"

namespace ouzel
{
    namespace audio
    {
        namespace mixer
        {
            Stream::Stream(Source& initSource):
                source(initSource)
            {
            }

            Stream::~Stream()
            {
                if (output) output->removeInput(this);
            }

            void Stream::setOutput(Bus* newOutput)
            {
                if (output) output->removeInput(this);
                output = newOutput;
                if (output) output->addInput(this);
            }

            void Stream::play(bool repeat)
            {
                playing = true;
                repeating = repeat;
            }

            void Stream::stop(bool shouldReset)
            {
                playing = false;
                if (shouldReset) reset();
            }
        }
    } // namespace audio
} // namespace ouzel
