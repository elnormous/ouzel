// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "Stream.hpp"
#include "Bus.hpp"
#include "Data.hpp"

namespace ouzel
{
    namespace audio
    {
        namespace mixer
        {
            Stream::Stream(Data& initData):
                data(initData)
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

            void Stream::play()
            {
                playing = true;
            }

            void Stream::stop(bool shouldReset)
            {
                playing = false;
                if (shouldReset) reset();
            }
        }
    } // namespace audio
} // namespace ouzel
