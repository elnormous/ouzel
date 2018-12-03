// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Bus.hpp"

namespace ouzel
{
    namespace audio
    {
        Bus::Bus()
        {
        }

        void Bus::addProcessor(Processor* processor)
        {
        }

        void Bus::removeProcessor(Processor* processor)
        {
        }

        void Bus::setOutput(Bus* newOutput)
        {
            output = newOutput;
        }
    } // namespace audio
} // namespace ouzel
