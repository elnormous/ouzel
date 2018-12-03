// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Processor.hpp"

namespace ouzel
{
    namespace audio
    {
        Processor::~Processor()
        {
            if (bus) bus->removeProcessor(this);
        }

        void Processor::setBus(Bus* newBus)
        {
            if (bus) bus->removeProcessor(this);
            bus = newBus;
            if (bus) bus->addProcessor(this);
        }
    } // namespace audio
} // namespace ouzel
