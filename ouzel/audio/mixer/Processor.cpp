// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "Processor.hpp"

namespace ouzel
{
    namespace audio
    {
        namespace mixer
        {
            Processor::~Processor()
            {
                if (bus) bus->removeProcessor(this);
            }
        }
    } // namespace audio
} // namespace ouzel
